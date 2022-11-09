#include "thread/Process.h"
#include "IO/Path.h"
#include "IO/Directory.h"
#include "data/Convert.h"
#include "data/StringArray.h"
#include "data/TimeSpan.h"
#include "diag/Trace.h"
#include "exception/Exception.h"
#include "thread/TickTimeout.h"

#if WIN32

#include <Windows.h>
#include <Psapi.h>
#include <TlHelp32.h>
////#pragma comment (lib, "User32.lib")
//#pragma comment (lib, "Advapi32.lib")
#else

#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>

#if __APPLE__

#include <sys/sysctl.h>

#endif

#include <pwd.h>

#endif

namespace Common {
    Process::Process() : Process(-1) {
    }

    Process::Process(int processId) {
        _id = processId;
        _redirectStdout = false;
        _waiting = 0;
#ifdef DEBUG
        showChildLog = true;
#endif
    }

    Process::~Process() {
    }

#if WIN32

    void Process::enableDebugPriv() const {
        HANDLE hToken;
        LUID luid;
        TOKEN_PRIVILEGES tkp;

        OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

        LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid);

        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Luid = luid;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), nullptr, nullptr);

        CloseHandle(hToken);
    }

#endif

#ifndef WIN32

    int readWithTimeout(int handle, char *data, uint maxlen, uint timeout) {
        int len = -1;
        if (handle != -1) {
            const int MAX_COUNT = 512;
            int red = 0;
            char revda[MAX_COUNT];
            int r_num = 0;
            int i;

            fd_set readfds;
            struct timeval tv_timeout;

            tv_timeout.tv_sec = timeout / 1000;
            tv_timeout.tv_usec = 1000 * (timeout % 1000);

            while (1) {
                FD_ZERO (&readfds);
                FD_SET (handle, &readfds);

                red = select(handle + 1, &readfds, nullptr, nullptr, &tv_timeout);
                if (red > 0) {
                    if (red > MAX_COUNT)
                        red = MAX_COUNT;

                    red = (int) ::read(handle, &revda, red);
//					Debug::writeFormatLine("readWithTimeout::read, red: %d", red);
                    if (red > 0) {
                        for (i = 0; i < red; i++) {
                            data[r_num] = revda[i];
                            r_num++;

                            if (r_num >= (int) maxlen)
                                return (r_num);
                        }
                    } else {
                        return (0);
                    }
                } else {
                    // timeout
                    return (-2);
                }
            }//end while
        }
        return len;
    }

#endif

    bool Process::start(const String &fileName, const String &arguments, Process *process) {
        if (fileName.isNullOrEmpty())
            throw ArgumentNullException("fileName");
#if WIN32
        STARTUPINFO si;
        memset(&si, 0, sizeof(si));
        si.cb = sizeof(si);
        HANDLE hStdOutRead = nullptr;
        HANDLE hStdOutWrite = nullptr;
        if (process != nullptr) {
            if (process->_redirectStdout) {
                SECURITY_ATTRIBUTES sa;
                // Set up the security attributes struct.
                sa.nLength = sizeof(SECURITY_ATTRIBUTES);
                sa.lpSecurityDescriptor = nullptr;
                //SECURITY_DESCRIPTOR sd;               //security information for pipes
                //InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
                //SetSecurityDescriptorDacl(&sd, true, nullptr, false);
                //sa.lpSecurityDescriptor = &sd;
                sa.bInheritHandle = TRUE;
                if (CreatePipe(&hStdOutRead, &hStdOutWrite, &sa, 0)) {
                    SetHandleInformation(hStdOutRead, HANDLE_FLAG_INHERIT, 0);
                    si.dwFlags |= STARTF_USESTDHANDLES;
                    si.hStdOutput = hStdOutWrite;
                    si.hStdError = hStdOutWrite;
                }
            }
        }
        //sj.hStdOutput
        PROCESS_INFORMATION pi;
        memset(&pi, 0, sizeof(pi));
        String clineStr = arguments != nullptr ? String::format("%s %s", fileName.c_str(), arguments.c_str()) : String(fileName);
        BOOL result = CreateProcess(nullptr, (char *) clineStr.c_str(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW,
                                    nullptr, nullptr, &si, &pi);
        if (result) {
            if (process != nullptr) {
                process->_id = pi.dwProcessId;

                if (hStdOutRead != nullptr) {
                    char buffer[128];
                    memset(buffer, 0, sizeof(buffer));
                    DWORD dwRead = 0, tdwAfail = 0, tdwLeft, dwAvail, ctr = 0;
                    while (PeekNamedPipe(hStdOutRead, buffer, 1, &dwRead, &dwAvail, &tdwLeft)) {
                        if (dwRead) {
                            if (ReadFile(hStdOutRead, buffer, sizeof(buffer) - 1, &dwRead, nullptr) && dwRead != 0) {
                                buffer[dwRead] = '\0';
                                process->_stdoutStr.append(buffer);

                                static String line;
                                line.append(buffer[0]);
                                if (buffer[0] == '\n') {
                                    ProcessOutputEventArgs e(line);
                                    process->_outputDelegates.invoke(process, &e);
                                    line.empty();
                                }
                            }
                        } else {
                            if (WaitForSingleObject(pi.hProcess, 0) == WAIT_OBJECT_0)
                                break;
                        }
                    }
                }

                if (process->_waiting != 0) {
                    WaitForSingleObject(pi.hProcess, process->_waiting);
                }
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
        }

        if (hStdOutRead != nullptr)
            CloseHandle(hStdOutRead);
        if (hStdOutWrite != nullptr)
            CloseHandle(hStdOutWrite);

        return result;
#else
        char **argv = nullptr;
        uint len = 0;
        if (arguments != nullptr) {
            String temp = arguments;
            StringArray as;
            Convert::splitStr(temp, ' ', as, '"');
            len = as.count();
            argv = new char *[len + 2];

            size_t tempLen = strlen(fileName) + 1;
            argv[0] = new char[tempLen];
            memset(argv[0], 0, tempLen);
            strcpy(argv[0], fileName);
            for (uint i = 1; i <= len; i++) {
                const String &valueStr = as[i - 1];
                size_t tempLen = valueStr.length() + 1;
                argv[i] = new char[tempLen];
                memset(argv[i], 0, tempLen);
                strcpy(argv[i], valueStr.c_str());
            }
            argv[len + 1] = nullptr;

//            for(uint i=0;i<len+1;i++)
//            {
//                const char* str = argv[i];
//                if(str != nullptr)
//                {
//                    Trace::info(String::format("argv[i = %d]: %s", i, str));
//                }
//            }
        }

        int pipefd[2];
        pipe(pipefd);

        pid_t child;
        if ((child = fork()) < 0) {
            Trace::error(String::format("Process::start.fork'%s %s' failed with error(%d): %s", fileName.c_str(),
                                        arguments.c_str(), errno, strerror(errno)));
            return false;
        }

        if (child == 0)      // child process
        {
#ifdef DEBUG
            if (process == nullptr ||
                (process != nullptr && process->showChildLog)) {
                Debug::writeFormatLine("Process::start(child process), file name: %s, arguments: %s, child pid: %d",
                                       fileName.c_str(), arguments.c_str(), getpid());
            }
#endif

            close(pipefd[0]);    // close reading end in the child

            dup2(pipefd[1], STDOUT_FILENO);  // send stdout to the pipe
            dup2(pipefd[1], STDERR_FILENO);  // send stderr to the pipe

            close(pipefd[1]);    // this descriptor is no longer needed

            int result = execvp(fileName, argv);
            if (argv != nullptr) {
                assert(len > 0);
                for (size_t i = 0; i <= len; i++)
                    delete[] argv[i];
                delete[] argv;
            }
            if (result == -1) {
                Trace::error(
                        String::format("Process::start.execvp'%s %s' failed with error(%d): %s", fileName.c_str(),
                                       arguments.c_str(),
                                       errno, strerror(errno)));
                return false;
            }
            return true;
        } else {
//        	Debug::writeFormatLine("Process::start(parent process), file name: %s, arguments: %s", fileName, arguments);

            close(pipefd[1]); // parent doesn't write

//            Debug::writeLine("after close(pipefd[1])");

            int timeout = 0;
            if (process != nullptr) {
//            	Debug::writeLine("before redirectStdout");

                process->_id = child;
                process->_name = Path::getFileName(fileName);

                if (process->_redirectStdout) {
                    String str;
                    char reading_buf[512];
                    if (process->_waiting != 0) {
                        while ((timeout = readWithTimeout(pipefd[0], reading_buf, 1, process->_waiting)) > 0) {
                            str.append(reading_buf[0]);

                            static String line;
                            line.append(reading_buf[0]);
                            if (reading_buf[0] == '\n') {
                                ProcessOutputEventArgs e(line);
                                process->_outputDelegates.invoke(process, &e);
                                line.empty();
                            }
                        }
                    } else {
                        while (read(pipefd[0], reading_buf, 1) > 0) {
                            str.append(reading_buf[0]);

                            static String line;
                            line.append(reading_buf[0]);
                            if (reading_buf[0] == '\n') {
                                ProcessOutputEventArgs e(line);
                                process->_outputDelegates.invoke(process, &e);
                                line.empty();
                            }
                        }
                    }
                    process->_stdoutStr = str;
                }
//                Debug::writeLine("after redirectStdout");
            }
            close(pipefd[0]);

            if (process != nullptr) {
                if (process->_waiting != 0) {
//                	Debug::writeFormatLine("start process->waitForExit, timeout: %d ms", process->_waiting);
                    if (timeout != -2)
                        process->waitForExit(process->_waiting);
//                    Debug::writeFormatLine("end process->waitForExit, timeout: %d ms", process->_waiting);
                    if (process->exist())
                        process->kill();
                }
            } else {
                if (!(argv != nullptr && strcmp(argv[len], "&") == 0)) {
                    int status;
                    while (waitpid(child, &status, 0) < 0) {
                        if (errno != EINTR) {
                            status = -1;
                            break;
                        }
                    }
                }
            }
        }
#endif
        return true;
    }

    bool Process::startByCmdString(const String &cmdString) {
#ifdef WIN32
        BOOL result = CreateProcess(nullptr, (char *) cmdString.c_str(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW,
                                    nullptr, nullptr, nullptr, nullptr);
        if (result) {
            return true;
        }
        return false;
#elif !IPHONE_OS
        return system(cmdString);
#else
        return false;
#endif
    }

    bool Process::getProcessById(int processId, Process &process) {
#if WIN32
        // Create toolhelp snapshot.
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 entry;
        ZeroMemory(&entry, sizeof(entry));
        entry.dwSize = sizeof(entry);

        // Walkthrough all processes.
        if (Process32First(snapshot, &entry)) {
            do {
                // Compare process.szExeFile based on format of name, i.e., trim file path
                // trim .exe if necessary, etc.
                //if (MatchProcessName(process.szExeFile, name))
                if (processId == entry.th32ProcessID) {
                    process._id = entry.th32ProcessID;
                    process._name = entry.szExeFile;
                    break;
                }
            } while (Process32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);

#elif __APPLE__
        int err = 0;
        struct kinfo_proc *proc_list = nullptr;
        size_t length = 0;

        static const int name[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};

        // Call sysctl with a nullptr buffer to get proper length
        err = sysctl((int *) name, (sizeof(name) / sizeof(*name)) - 1, nullptr, &length, nullptr, 0);
        if (err) return false;

        // Allocate buffer
        proc_list = (struct kinfo_proc *) malloc(length);
        if (!proc_list) return false;

        // Get the actual process list
        err = sysctl((int *) name, (sizeof(name) / sizeof(*name)) - 1, proc_list, &length, nullptr, 0);
        if (err) return false;

        int proc_count = (int) length / sizeof(struct kinfo_proc);

        // use getpwuid_r() if you want to be thread-safe

        for (int i = 0; i < proc_count; i++) {
            pid_t pid = proc_list[i].kp_proc.p_pid;
            const char *first = proc_list[i].kp_proc.p_comm;
            if (pid == processId) {
                process._id = pid;
                process._name = first;
                break;
            }
        }

        free(proc_list);
#else
        DIR* dir;
        struct dirent* ent;
        char* endptr;
        char buf[512];
        
        if (!(dir = opendir("/proc"))) {
            perror("can't open /proc");
            return false;
        }
        
        while((ent = readdir(dir)) != nullptr) {
            /* if endptr is not a null character, the directory is not
             * entirely numeric, so ignore it */
            long lpid = strtol(ent->d_name, &endptr, 10);
            if (*endptr != '\0') {
                continue;
            }
            
            /* try to open the cmdline file */
            snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
            FILE* fp = fopen(buf, "r");
            
            if (fp) {
                if (fgets(buf, sizeof(buf), fp) != nullptr) {
                    /* check the first token in the file, the program name */
                    const char* first = strtok(buf, " ");
                    const String fileName = Path::getFileName(first);
                    const String pname = fileName.isNullOrEmpty() ? String(first) : fileName;
                    //                    Debug::writeFormatLine("process name: %s", pname.c_str());
                    if (processId == (pid_t)lpid) {
                        process._id = (pid_t)lpid;
                        process._name = pname;
                        break;
                    }
                }
                fclose(fp);
            }
        }
        
        closedir(dir);
#endif
        return process._id > 0;
    }

    bool Process::getProcessByName(const char *processName, Processes &processes) {
#if WIN32
        // Create toolhelp snapshot.
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        PROCESSENTRY32 entry;
        ZeroMemory(&entry, sizeof(entry));
        entry.dwSize = sizeof(entry);

        // Walkthrough all processes.
        if (Process32First(snapshot, &entry)) {
            do {
                // Compare process.szExeFile based on format of name, i.e., trim file path
                // trim .exe if necessary, etc.
                if (strcmp(entry.szExeFile, processName) == 0) {
                    Process *process = new Process(entry.th32ProcessID);
                    process->_name = processName;
                    processes.add(process);
                }
            } while (Process32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);

#elif __APPLE__
        int err = 0;
        struct kinfo_proc *proc_list = nullptr;
        size_t length = 0;

        static const int name[] = {CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0};

        // Call sysctl with a nullptr buffer to get proper length
        err = sysctl((int *) name, (sizeof(name) / sizeof(*name)) - 1, nullptr, &length, nullptr, 0);
        if (err) return false;

        // Allocate buffer
        proc_list = (struct kinfo_proc *) malloc(length);
        if (!proc_list) return false;

        // Get the actual process list
        err = sysctl((int *) name, (sizeof(name) / sizeof(*name)) - 1, proc_list, &length, nullptr, 0);
        if (err) return false;

        int proc_count = (int) length / sizeof(struct kinfo_proc);

        // use getpwuid_r() if you want to be thread-safe

        for (int i = 0; i < proc_count; i++) {
            pid_t pid = proc_list[i].kp_proc.p_pid;
            const char *first = proc_list[i].kp_proc.p_comm;
            if (!strcmp(first, processName)) {
                Process *process = new Process((pid_t) pid);
                process->_name = processName;
                processes.add(process);
            }
        }

        free(proc_list);
#else
        DIR* dir;
        struct dirent* ent;
        char* endptr;
        char buf[512];
        
        if (!(dir = opendir("/proc"))) {
            perror("can't open /proc");
            return false;
        }
        
        while((ent = readdir(dir)) != nullptr) {
            /* if endptr is not a null character, the directory is not
             * entirely numeric, so ignore it */
            long lpid = strtol(ent->d_name, &endptr, 10);
            if (*endptr != '\0') {
                continue;
            }
            
            /* try to open the cmdline file */
            snprintf(buf, sizeof(buf), "/proc/%ld/cmdline", lpid);
            FILE* fp = fopen(buf, "r");
            
            if (fp) {
                if (fgets(buf, sizeof(buf), fp) != nullptr) {
                    /* check the first token in the file, the program name */
                    const char* first = strtok(buf, " ");
                    const String fileName = Path::getFileName(first);
                    const String pname = fileName.isNullOrEmpty() ? String(first) : fileName;
//                    Debug::writeFormatLine("process name: %s", pname.c_str());
                    if (pname == processName) {
                        Process* process = new Process((pid_t)lpid);
                        process->_name = processName;
                        processes.add(process);
                    }
                }
                fclose(fp);
            }
        }
        
        closedir(dir);
#endif
        return processes.count() > 0;
    }

    bool Process::getProcessByName(const String &processName, Processes &processes) {
        return getProcessByName(processName.c_str(), processes);
    }

    bool Process::waitForExit(int milliseconds) const {
        if (_id > 0) {
#if WIN32
            enableDebugPriv();
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _id);
            if (hProcess != nullptr) {
                WaitForSingleObject(hProcess, milliseconds);
                CloseHandle(hProcess);

                return true;
            }
#else
            TickTimeout::msdelay(milliseconds, isProcessEnded, (void *) this, 10);
            return !exist();
#endif
        }
        return false;
    }

    bool Process::isProcessEnded(void *parameter) {
        Process *process = static_cast<Process *>(parameter);
        assert(process);
        return !process->exist();
    }

    bool Process::waitForProcessExit(int processId, int milliseconds) {
        Process process;
        if (getProcessById(processId, process)) {
            return process.waitForExit(milliseconds);
        }
        return true;
    }

    bool Process::getCurrentProcess(Process &process) {
#if WIN32
        DWORD processId = ::GetCurrentProcessId();
        if (processId != -1) {
            process._id = processId;
            return true;
        }
#else
        pid_t pid = getpid();
        if (pid > 0) {
            process._id = pid;
            return true;
        }
#endif
        return false;
    }

    int Process::getCurrentProcessId() {
        Process process;
        getCurrentProcess(process);
        return process._id;
    }

    bool Process::kill() {
        if (_id > 0) {
#if WIN32
            enableDebugPriv();
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _id);
            if (hProcess != nullptr) {
                bool result = TerminateProcess(hProcess, -1) != 0;
                if (!result) {
                    DWORD error = GetLastError();
                    Debug::writeFormatLine("Failed to TerminateProcess, error: %d", error);
                }
                CloseHandle(hProcess);
                _id = -1;
                return result;
            }
#else
            bool result = ::kill(_id, SIGKILL) == 0;
            _id = -1;
            return result;
#endif
        }
        return false;
    }

    bool Process::stop(int milliseconds) {
        if (_id > 0) {
#if WIN32
            return kill();
#else
            bool result = ::kill(_id, SIGTERM) == 0;
            waitForExit(milliseconds);
            return result;
#endif
        }
        return false;
    }

    bool Process::closeMainWindow() const {
#if WIN32
        HWND main = (HWND) mainWindow();
        if (main != NULL) {
            return PostMessage(main, WM_CLOSE, NULL, NULL) != 0;
        }
#else
#endif
        return false;
    }

    int Process::getId() const {
        return _id;
    }

    int Process::id() const {
        return _id;
    }

    const String &Process::name() const {
        return _name;
    }

    const String &Process::stdoutStr() const {
        return _stdoutStr;
    }

    bool Process::redirectStdout() const {
        return _redirectStdout;
    }

    void Process::setRedirectStdout(bool redirectStdout) {
        _redirectStdout = redirectStdout;
    }

    bool Process::waitingTimeout() const {
        return _waiting != 0;
    }

    void Process::setWaitingTimeout(int milliseconds) {
        _waiting = milliseconds;
    }

    void Process::setWaitingTimeout(TimeSpan timeout) {
        if (timeout != TimeSpan::Zero) {
            setWaitingTimeout((int) timeout.totalMilliseconds());
        }
    }

#if WIN32
    struct ProcessWindows {
        DWORD dwCurrentProcessId;
        HWND hCurrentProcessWin;
    };

    BOOL IsMainWindow(HWND hWnd) {
        return (!(GetWindow(hWnd, GW_OWNER) != nullptr) && IsWindowVisible(hWnd));
    }

    BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
        DWORD dwProcessId = 0;
        GetWindowThreadProcessId(hWnd, &dwProcessId);

        ProcessWindows *ppw = (ProcessWindows *) lParam;
        if (ppw != nullptr) {
            if (ppw->dwCurrentProcessId == dwProcessId && IsMainWindow(hWnd)) {
                ppw->hCurrentProcessWin = hWnd;
                return FALSE;
            }
        }
        return TRUE;
    }

#endif

    void *Process::mainWindow() const {
#if WIN32
        ProcessWindows pw;
        memset(&pw, 0, sizeof(pw));
        pw.dwCurrentProcessId = _id;
        EnumWindows(EnumWindowsProc, (LPARAM) &pw);
        return (void *) pw.hCurrentProcessWin;
#else
        // todo: return the main window.
        throw NotImplementedException("Process::mainWindow");
#endif
    }

    bool Process::exist() const {
        if (_id > 0) {
#if WIN32
            enableDebugPriv();
            HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _id);
            return (hProcess != nullptr);
#elif __linux__
            // Issue a kill(2) system call with 0 as the signal. If the call succeeds, it means that a process with this pid exists.
            // If the call fails and errno is set to ESRCH, a process with such a pid does not exist.
            if(::kill(_id, 0) == 0)
                return true;
            else
            {
                // The calling process does not have permission to send the signal to any of the target processes.
                if(errno == EPERM)
                    return getpgid(_id) != -1;
                else
                    return errno != ESRCH;
            }
#else
            return getpgid(_id) != -1;
#endif
        }
        return false;
    }

    Delegates *Process::outputDelegates() {
        return &_outputDelegates;
    }
}
