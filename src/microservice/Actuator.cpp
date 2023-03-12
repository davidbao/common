//
//  Actuator.cpp
//  common
//
//  Created by baowei on 2020/3/4.
//  Copyright (c) 2020 com. All rights reserved.
//

#include "microservice/Actuator.h"
#include "IO/Metrics.h"
#include "IO/Directory.h"
#include "diag/Process.h"
#include "thread/TickTimeout.h"
#include "system/Environment.h"
#include "system/ServiceFactory.h"
#include "configuration/ConfigService.h"
#include "microservice/SummerApplication.h"
#include "microservice/ServiceRegister.h"

namespace Microservice {
    IActuator::IActuator() {
    }

    IActuator::~IActuator() {
    }

    HttpStatus IActuator::onActuator(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            const Url &baseUrl = request.url;

            JsonNode rootNode, linkNode("_links");

            JsonNode selfNode("self");
            selfNode.add(JsonNode("href", baseUrl));
            selfNode.add(JsonNode("templated", false));
            linkNode.add(selfNode);

            JsonNode infoNode("info");
            infoNode.add(JsonNode("href", Url(baseUrl, "info")));
            infoNode.add(JsonNode("templated", false));
            linkNode.add(infoNode);

            JsonNode healthComponentNode("health-component");
            healthComponentNode.add(JsonNode("href", Url(baseUrl, "health/{component}")));
            healthComponentNode.add(JsonNode("templated", true));
            linkNode.add(healthComponentNode);

            JsonNode healthComponentInstanceNode("health-component-instance");
            healthComponentInstanceNode.add(JsonNode("href", Url(baseUrl, "health/{component}/{instance}")));
            healthComponentInstanceNode.add(JsonNode("templated", true));
            linkNode.add(healthComponentInstanceNode);

            JsonNode healthNode("health");
            healthNode.add(JsonNode("href", Url(baseUrl, "health")));
            healthNode.add(JsonNode("templated", false));
            linkNode.add(healthNode);

            JsonNode metricsNode("metrics");
            metricsNode.add(JsonNode("href", Url(baseUrl, "metrics")));
            metricsNode.add(JsonNode("templated", false));
            linkNode.add(metricsNode);

            JsonNode metrics_requiredMetricNameNode("metrics-requiredMetricName");
            metrics_requiredMetricNameNode.add(JsonNode("href", Url(baseUrl, "metrics/{requiredMetricName}")));
            metrics_requiredMetricNameNode.add(JsonNode("templated", true));
            linkNode.add(metrics_requiredMetricNameNode);

            JsonNode envNode("env");
            envNode.add(JsonNode("href", Url(baseUrl, "env")));
            envNode.add(JsonNode("templated", false));
            linkNode.add(envNode);

            JsonNode env_toMatchNode("env-toMatch");
            env_toMatchNode.add(JsonNode("href", Url(baseUrl, "env/{toMatch}")));
            env_toMatchNode.add(JsonNode("templated", true));
            linkNode.add(env_toMatchNode);

            JsonNode serviceRegistryNode("service-registry");
            serviceRegistryNode.add(JsonNode("href", Url(baseUrl, "service-registry")));
            serviceRegistryNode.add(JsonNode("templated", false));
            linkNode.add(serviceRegistryNode);

            rootNode.add(linkNode);
            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onInfo(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            IConfigService *cs = factory->getService<IConfigService>();
            assert(cs);
            Application *app = Application::instance();
            assert(app);

            String name, description, version;
            if (!cs->getProperty("summer.application.name", name))
                name = app->name();
            if (!cs->getProperty("summer.application.description", description))
                description = name;
            if (!cs->getProperty("summer.application.version", version))
                version = Version::Version1_0.toString();

            JsonNode rootNode, buildNode("build");
            buildNode.add(JsonNode("name", name));
            buildNode.add(JsonNode("description", description));
            buildNode.add(JsonNode("version", version));
            buildNode.add(JsonNode("time", app->startTime()));
            rootNode.add(buildNode);
            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onHealth(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;
            rootNode.add(JsonNode("status", "UP"));

            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            IConfigService *cs = factory->getService<IConfigService>();
            assert(cs);
            Application *app = Application::instance();
            assert(app);

            String detailsStr;
            if (cs->getProperty("management.endpoint.health.show-details", detailsStr) &&
                detailsStr.toLower() == "always") {
                JsonNode detailsNode("details");

                // diskSpace
                JsonNode diskSpaceNode("diskSpace");
                const String diskStatPath = app->fullFileName();
                uint64_t free = DiskStat::getAvailable(diskStatPath);
                static const uint64_t threshold = 10 * 1024 * 1024;  // default 10M
                diskSpaceNode.add(JsonNode("status", free >= threshold ? "UP" : "DOWN"));
                JsonNode dsDetailsNode("details");
                dsDetailsNode.add(JsonNode("total", DiskStat::getTotal(diskStatPath)));
                dsDetailsNode.add(JsonNode("free", free));
                dsDetailsNode.add(JsonNode("threshold", threshold));
                diskSpaceNode.add(dsDetailsNode);
                detailsNode.add(diskSpaceNode);

                // refreshScope
                JsonNode refreshScopeNode("refreshScope");
                refreshScopeNode.add(JsonNode("status", "UP"));
                detailsNode.add(refreshScopeNode);

                // discoveryComposite
                JsonNode discoveryCompositeNode("discoveryComposite");
                discoveryCompositeNode.add(JsonNode("status", "UP"));
                JsonNode dcDetailsNode("details");
                JsonNode discoveryClientNode("discoveryClient");
                discoveryClientNode.add(JsonNode("status", "UP"));
                JsonNode dccDetailsNode("details");
                JsonNode servicesNode("services", JsonNode::TypeArray);
                StringArray serviceIds;
                IServiceRegister *ss = factory->getService<IServiceRegister>();
                assert(ss);
                ss->getServiceIds(serviceIds);
                for (size_t i = 0; i < serviceIds.count(); i++) {
                    const String &serviceId = serviceIds[i];
                    servicesNode.add(JsonNode("item", serviceId));
                }
                dccDetailsNode.add(servicesNode);
                discoveryClientNode.add(dccDetailsNode);

                dcDetailsNode.add(discoveryClientNode);
                discoveryCompositeNode.add(dcDetailsNode);
                detailsNode.add(discoveryCompositeNode);

                // service health node
                JsonNode shNode;
                if (ss->getHealthNode(shNode)) {
                    detailsNode.add(shNode);
                }

                // hystrix
                JsonNode hystrixNode("hystrix");
                hystrixNode.add(JsonNode("status", "UP"));
                detailsNode.add(hystrixNode);

                rootNode.add(detailsNode);
            }

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onMetrics(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;
            StringArray names({"jvm.memory.max",
                               "jvm.threads.states",
                               "http.server.requests",
                               "process.files.max",
                               "system.load.average.1m",
                               "jvm.memory.used",
                               "jvm.memory.committed",
                               "system.cpu.count",
                               "logback.events",
                               "jvm.buffer.memory.used",
                               "jvm.threads.daemon",
                               "system.cpu.usage",
                               "jvm.threads.live",
                               "jvm.threads.peak",
                               "process.uptime",
                               "process.cpu.usage",
                               "jvm.classes.loaded",
                               "jvm.classes.unloaded",
                               "process.files.open",
                               "jvm.buffer.count",
                               "jvm.buffer.total.capacity",
                               "process.start.time"});
            rootNode.add(JsonNode("names", names));
            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onSystemCPUCount(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            rootNode.add(JsonNode("name", "system.cpu.count"));
            rootNode.add(JsonNode("description", "The number of processors available to the application"));
            rootNode.add(JsonNode("baseUnit", ""));

            JsonNode measurementsNode("measurements", JsonNode::TypeArray);
            JsonNode measurementNode;
            measurementNode.add(JsonNode("statistic", "VALUE"));
            measurementNode.add(JsonNode("value", CPUStat::numCPUs()));
            measurementsNode.add(measurementNode);
            rootNode.add(measurementsNode);

            rootNode.add(JsonNode("availableTags", StringArray()));

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onProcessUptime(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            rootNode.add(JsonNode("name", "process.uptime"));
            rootNode.add(JsonNode("description", "The uptime of the application"));
            rootNode.add(JsonNode("baseUnit", "seconds"));

            JsonNode measurementsNode("measurements", JsonNode::TypeArray);
            JsonNode measurementNode;
            measurementNode.add(JsonNode("statistic", "VALUE"));
            Application *app = Application::instance();
            assert(app);
            measurementNode.add(JsonNode("value", app->elapsedTime().totalSeconds()));
            measurementsNode.add(measurementNode);
            rootNode.add(measurementsNode);

            rootNode.add(JsonNode("availableTags", StringArray()));

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onProcessCPUUsage(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            rootNode.add(JsonNode("name", "process.cpu.usage"));
            rootNode.add(JsonNode("description", "The \"recent cpu usage\" for the application"));
            rootNode.add(JsonNode("baseUnit", ""));

            JsonNode measurementsNode("measurements", JsonNode::TypeArray);
            JsonNode measurementNode;
            measurementNode.add(JsonNode("statistic", "VALUE"));
            double usage = CPUStat::processUsage();
            if (usage == 0.0)
                usage = 0.001;
            measurementNode.add(JsonNode("value", usage));
            measurementsNode.add(measurementNode);
            rootNode.add(measurementsNode);

            rootNode.add(JsonNode("availableTags", StringArray()));

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onSystemCPUUsage(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            rootNode.add(JsonNode("name", "system.cpu.usage"));
            rootNode.add(JsonNode("description", "The \"recent cpu usage\" for the whole system"));
            rootNode.add(JsonNode("baseUnit", ""));

            JsonNode measurementsNode("measurements", JsonNode::TypeArray);
            JsonNode measurementNode;
            measurementNode.add(JsonNode("statistic", "VALUE"));
            double usage = CPUStat::usage();
            if (usage == 0.0)
                usage = 0.001;
            measurementNode.add(JsonNode("value", usage));
            measurementsNode.add(measurementNode);
            rootNode.add(measurementsNode);

            rootNode.add(JsonNode("availableTags", StringArray()));

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onThreadsLive(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            rootNode.add(JsonNode("name", "jvm.threads.live"));
            rootNode.add(JsonNode("description",
                                  "The current number of live threads including both daemon and non-daemon threads"));
            rootNode.add(JsonNode("baseUnit", "threads"));

            JsonNode measurementsNode("measurements", JsonNode::TypeArray);
            JsonNode measurementNode;
            measurementNode.add(JsonNode("statistic", "VALUE"));
            measurementNode.add(JsonNode("value", ThreadStat::live()));
            measurementsNode.add(measurementNode);
            rootNode.add(measurementsNode);

            rootNode.add(JsonNode("availableTags", StringArray()));

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onThreadsPeak(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            rootNode.add(JsonNode("name", "jvm.threads.peak"));
            rootNode.add(JsonNode("description",
                                  "The peak live thread count since the application started or peak was reset"));
            rootNode.add(JsonNode("baseUnit", "threads"));

            JsonNode measurementsNode("measurements", JsonNode::TypeArray);
            JsonNode measurementNode;
            measurementNode.add(JsonNode("statistic", "VALUE"));
            measurementNode.add(JsonNode("value", ThreadStat::peak()));
            measurementsNode.add(measurementNode);
            rootNode.add(measurementsNode);

            rootNode.add(JsonNode("availableTags", StringArray()));

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onThreadsDaemon(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            rootNode.add(JsonNode("name", "jvm.threads.daemon"));
            rootNode.add(JsonNode("description", "The current number of live daemon threads"));
            rootNode.add(JsonNode("baseUnit", "threads"));

            JsonNode measurementsNode("measurements", JsonNode::TypeArray);
            JsonNode measurementNode;
            measurementNode.add(JsonNode("statistic", "VALUE"));
            measurementNode.add(JsonNode("value", ThreadStat::daemon()));
            measurementsNode.add(measurementNode);
            rootNode.add(measurementsNode);

            rootNode.add(JsonNode("availableTags", StringArray()));

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onMemoryMax(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            rootNode.add(JsonNode("name", "jvm.memory.max"));
            rootNode.add(JsonNode("description",
                                  "The maximum amount of memory in bytes that can be used for memory management"));
            rootNode.add(JsonNode("baseUnit", "bytes"));

            JsonNode measurementsNode("measurements", JsonNode::TypeArray);
            JsonNode measurementNode;
            measurementNode.add(JsonNode("statistic", "VALUE"));
            int64_t value = 0;
            String tag = request.getPropValue("tag");
            if (tag == "area:heap")
                value = MemoryStat::maxHeap();
            else if (tag == "area:nonheap")
                value = MemoryStat::maxNonHeap();
            else
                value = MemoryStat::max();
            measurementNode.add(JsonNode("value", value));
            measurementsNode.add(measurementNode);
            rootNode.add(measurementsNode);

            JsonNode availableTagsNode(JsonNode::TypeArray);
            JsonNode idNode;
            idNode.add(JsonNode("tag", "id"));
            StringArray heapValues({"PS Eden Space", "PS Survivor Space", "PS Old Gen"});
            StringArray nonHeapValues({"Metaspace", "Compressed Class Space", "Code Cache"});
            if (tag == "area:heap")
                idNode.add(JsonNode("values", heapValues));
            else if (tag == "area:nonheap")
                idNode.add(JsonNode("values", nonHeapValues));
            else {
                JsonNode areaNode;
                areaNode.add(JsonNode("tag", "area"));
                StringArray areaValues({"heap", "nonheap"});
                areaNode.add(JsonNode("values", areaValues));
                availableTagsNode.add(areaNode);

                StringArray values;
                values.addRange(heapValues);
                values.addRange(nonHeapValues);
                idNode.add(JsonNode("values", values));
            }
            availableTagsNode.add(idNode);
            rootNode.add(availableTagsNode);

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onMemoryUsed(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            rootNode.add(JsonNode("name", "jvm.memory.used"));
            rootNode.add(JsonNode("description", "The amount of used memory"));
            rootNode.add(JsonNode("baseUnit", "bytes"));

            JsonNode measurementsNode("measurements", JsonNode::TypeArray);
            JsonNode measurementNode;
            measurementNode.add(JsonNode("statistic", "VALUE"));
            int64_t value = 0;
            String tag = request.getPropValue("tag");
//            area:nonheap,id:Metaspace
            StringMap tags;
            StringArray texts;
            StringArray::parse(tag, texts, ',');
            for (size_t i = 0; i < texts.count(); i++) {
                const String &text = texts[i];
                StringArray texts2;
                StringArray::parse(text, texts2, ':');
                if (texts2.count() == 2) {
                    tags.add(texts2[0], texts2[1]);
                }
            }
            String area = tags["area"];
            if (area == "heap")
                value = MemoryStat::usedHeap();
            else if (area == "nonheap") {
                value = tags["id"] == "Metaspace" ? 0 : MemoryStat::usedNonHeap();
            } else {
                value = MemoryStat::used();
            }

            measurementNode.add(JsonNode("value", value));
            measurementsNode.add(measurementNode);
            rootNode.add(measurementsNode);

            JsonNode availableTagsNode(JsonNode::TypeArray);
            JsonNode idNode;
            idNode.add(JsonNode("tag", "id"));
            StringArray heapValues({"PS Eden Space", "PS Survivor Space", "PS Old Gen"});
            StringArray nonHeapValues({"Metaspace", "Compressed Class Space", "Code Cache"});
            if (area == "heap")
                idNode.add(JsonNode("values", heapValues));
            else if (area == "nonheap") {
                idNode.add(JsonNode("values", nonHeapValues));
            } else {
                JsonNode areaNode;
                areaNode.add(JsonNode("tag", "area"));
                StringArray areaValues({"heap", "nonheap"});
                areaNode.add(JsonNode("values", areaValues));
                availableTagsNode.add(areaNode);

                StringArray values;
                values.addRange(heapValues);
                values.addRange(nonHeapValues);
                idNode.add(JsonNode("values", values));
            }
            if (tags["id"] == "Metaspace") {
                rootNode.add(JsonNode("availableTags", StringArray()));
            } else {
                availableTagsNode.add(idNode);
                rootNode.add(availableTagsNode);
            }

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onMemoryCommitted(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            rootNode.add(JsonNode("name", "jvm.memory.committed"));
            rootNode.add(JsonNode("description",
                                  "The amount of memory in bytes that is committed for the Java virtual machine to use"));
            rootNode.add(JsonNode("baseUnit", "bytes"));

            JsonNode measurementsNode("measurements", JsonNode::TypeArray);
            JsonNode measurementNode;
            measurementNode.add(JsonNode("statistic", "VALUE"));
            int64_t value = 0;
            String tag = request.getPropValue("tag");
            if (tag == "area:heap")
                value = MemoryStat::committedHeap();
            else if (tag == "area:nonheap")
                value = MemoryStat::committedNonHeap();
            else
                value = MemoryStat::committed();
            measurementNode.add(JsonNode("value", value));
            measurementsNode.add(measurementNode);
            rootNode.add(measurementsNode);

            JsonNode availableTagsNode(JsonNode::TypeArray);
            JsonNode idNode;
            idNode.add(JsonNode("tag", "id"));
            StringArray heapValues({"PS Eden Space", "PS Survivor Space", "PS Old Gen"});
            StringArray nonHeapValues({"Metaspace", "Compressed Class Space", "Code Cache"});
            if (tag == "area:heap")
                idNode.add(JsonNode("values", heapValues));
            else if (tag == "area:nonheap")
                idNode.add(JsonNode("values", nonHeapValues));
            else {
                JsonNode areaNode;
                areaNode.add(JsonNode("tag", "area"));
                StringArray areaValues({"heap", "nonheap"});
                areaNode.add(JsonNode("values", areaValues));
                availableTagsNode.add(areaNode);

                StringArray values;
                values.addRange(heapValues);
                values.addRange(nonHeapValues);
                idNode.add(JsonNode("values", values));
            }
            availableTagsNode.add(idNode);
            rootNode.add(availableTagsNode);

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onEnvironment(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            Application *app = Application::instance();
            assert(app);
            IConfigService *cs = factory->getService<IConfigService>();
            assert(cs);
            String profile = "none";
            cs->getProperty("summer.profiles.active", profile);
            StringArray profiles{profile};
            rootNode.add(JsonNode("activeProfiles", profiles));

            JsonNode propertySourcesNode("propertySources", JsonNode::TypeArray);

            // server.port
            {
                JsonNode node;
                node.add(JsonNode("name", "server.ports"));
                JsonNode propertiesNode("properties");
                JsonNode serverPortNode("local.server.port");
                int serverPort = 0;
                cs->getProperty("server.port", serverPort);
                serverPortNode.add(JsonNode("value", serverPort));
                propertiesNode.add(serverPortNode);
                JsonNode managementPortNode("local.management.port");
                int managementPort = 0;
                cs->getProperty("management.server.port", managementPort);
                managementPortNode.add(JsonNode("value", managementPort));
                propertiesNode.add(managementPortNode);
                node.add(propertiesNode);
                propertySourcesNode.add(node);
            }

            // commandLineArgs
            {
                JsonNode node;
                node.add(JsonNode("name", "commandLineArgs"));
                JsonNode propertiesNode("properties");

                const Application::Arguments &argv = app->arguments();

                StringArray keys;
                argv.keys(keys);
                for (size_t i = 0; i < keys.count(); i++) {
                    const String &key = keys[i];
                    String value;
                    if (argv.at(key, value)) {
                        JsonNode propNode(key);
                        propNode.add(JsonNode("value", value));
                        propertiesNode.add(propNode);
                    }
                }
                node.add(propertiesNode);
                propertySourcesNode.add(node);
            }

            // systemProperties
            {
                JsonNode node;
                node.add(JsonNode("name", "systemProperties"));
                JsonNode propertiesNode("properties");
                StringMap properties({
//                    {"user.country", UserStat::country()},
//                    {"user.country.format", UserStat::countryFormat()},
                                             {"user.dir",       UserStat::dir()},
                                             {"user.name",      UserStat::name()},
                                             {"user.language",  UserStat::language()},
                                             {"user.timezone",  UserStat::timezone()},
                                             {"user.home",      UserStat::home()},
                                             {"os.arch",        OSStat::arch()},
                                             {"os.name",        OSStat::name()},
                                             {"os.version",     OSStat::version()},
                                             {"@appId",         app->name()},
                                             {"file.encoding",  "UTF-8"},
                                             {"file.separator", String(Path::DirectorySeparatorChar)},
                                             {"io.tmpdir",      Path::getTempPath()},
//                    {"line.separator", String::NewLine},
                                             {"PID",            ((Int32) Process::getCurrentProcessId()).toString()}
                                     });
                StringArray keys;
                properties.keys(keys);
                for (size_t i = 0; i < keys.count(); i++) {
                    const String &key = keys[i];
                    String value;
                    if (properties.at(key, value)) {
                        JsonNode propNode(key);
                        propNode.add(JsonNode("value", value));
                        propertiesNode.add(propNode);
                    }
                }
                node.add(propertiesNode);
                propertySourcesNode.add(node);
            }

            // systemEnvironment
            {
                JsonNode node;
                node.add(JsonNode("name", "systemEnvironment"));
                JsonNode propertiesNode("properties");

                StringMap variables;
                Environment::getVariables(variables);

                StringArray keys;
                variables.keys(keys);
                for (size_t i = 0; i < keys.count(); i++) {
                    const String &key = keys[i];
                    String value;
                    if (variables.at(key, value)) {
                        JsonNode propNode(key);
                        propNode.add(JsonNode("value", value));
                        propNode.add(
                                JsonNode("origin", String::format("System Environment Property \"%s\"", key.c_str())));
                        propertiesNode.add(propNode);
                    }
                }
                node.add(propertiesNode);
                propertySourcesNode.add(node);
            }

            // applicationConfig profile
            {
                JsonNode node;
                String fileName = String::format("application-%s.yml", profile.c_str());
                node.add(JsonNode("name", String::format("applicationConfig: [apppath:./%s]", fileName.c_str())));
                JsonNode propertiesNode("properties");

                String ymlFileName;
                const String appPath = Path::getAppPath();
                ymlFileName = Path::combine(appPath, fileName);
                if (!File::exists(ymlFileName)) {
                    ymlFileName = Path::combine(app->rootPath(), fileName);
                }

                YmlNode doc;
                if (YmlNode::parseFile(ymlFileName, doc)) {
                    YmlNode::Properties properties;
                    doc.getProperties(properties);

                    StringArray keys;
                    properties.keys(keys);
                    for (size_t i = 0; i < keys.count(); i++) {
                        const String &key = keys[i];
                        String value;
                        if (properties.at(key, value) && !value.isNullOrEmpty()) {
                            JsonNode propNode(key);
                            propNode.add(JsonNode("value", value));
                            propNode.add(
                                    JsonNode("origin", String::format("app path resource [%s]", fileName.c_str())));
                            propertiesNode.add(propNode);
                        }
                    }
                    node.add(propertiesNode);
                    propertySourcesNode.add(node);
                }
            }

            // applicationConfig
            {
                JsonNode node;
                String fileName = "application.yml";
                node.add(JsonNode("name", String::format("applicationConfig: [apppath:./%s]", fileName.c_str())));
                JsonNode propertiesNode("properties");

                String ymlFileName;
                const String appPath = Path::getAppPath();
                ymlFileName = Path::combine(appPath, fileName);
                if (!File::exists(ymlFileName)) {
                    ymlFileName = Path::combine(app->rootPath(), fileName);
                }

                YmlNode doc;
                if (YmlNode::parseFile(ymlFileName, doc)) {
                    YmlNode::Properties properties;
                    doc.getProperties(properties);

                    StringArray keys;
                    properties.keys(keys);
                    for (size_t i = 0; i < keys.count(); i++) {
                        const String &key = keys[i];
                        String value;
                        if (properties.at(key, value) && !value.isNullOrEmpty()) {
                            JsonNode propNode(key);
                            propNode.add(JsonNode("value", value));
                            propNode.add(
                                    JsonNode("origin", String::format("app path resource [%s]", fileName.c_str())));
                            propertiesNode.add(propNode);
                        }
                    }
                    node.add(propertiesNode);
                    propertySourcesNode.add(node);
                }
            }

            // summerCloudClientHostInfo
            {
                JsonNode node;
                node.add(JsonNode("name", "summerCloudClientHostInfo"));
                JsonNode propertiesNode("properties");

                String ipAddress, hostName;
                cs->getProperty("summer.cloud.client.ip-address", ipAddress);
                cs->getProperty("summer.cloud.client.hostname", hostName);
                StringMap properties({
                                             {"summer.cloud.client.hostname",   hostName},
                                             {"summer.cloud.client.ip-address", ipAddress}
                                     });
                StringArray keys;
                properties.keys(keys);
                for (size_t i = 0; i < keys.count(); i++) {
                    const String &key = keys[i];
                    String value;
                    if (properties.at(key, value)) {
                        JsonNode propNode(key);
                        propNode.add(JsonNode("value", value));
                        propertiesNode.add(propNode);
                    }
                }
                node.add(propertiesNode);
                propertySourcesNode.add(node);
            }

            // defaultProperties
            {
                JsonNode node;
                node.add(JsonNode("name", "defaultProperties"));
                JsonNode propertiesNode("properties");
                StringMap properties({});
                StringArray keys;
                properties.keys(keys);
                for (size_t i = 0; i < keys.count(); i++) {
                    const String &key = keys[i];
                    String value;
                    if (properties.at(key, value)) {
                        JsonNode propNode(key);
                        propNode.add(JsonNode("value", value));
                        propertiesNode.add(propNode);
                    }
                }
                node.add(propertiesNode);
                propertySourcesNode.add(node);
            }

            rootNode.add(propertySourcesNode);

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    HttpStatus IActuator::onProcessId(const HttpRequest &request, HttpResponse &response) {
        if (request.method == HttpMethod::Options)
            return HttpStatus::HttpOk;
        else if (request.method == HttpMethod::Get) {
            JsonNode rootNode;

            JsonNode propertyNode("property");
            propertyNode.add(JsonNode("source", "systemProperties"));
            propertyNode.add(JsonNode("value", Process::getCurrentProcessId()));
            rootNode.add(propertyNode);

            ServiceFactory *factory = ServiceFactory::instance();
            assert(factory);
            IConfigService *cs = factory->getService<IConfigService>();
            assert(cs);
            String profile = "none";
            cs->getProperty("summer.profiles.active", profile);
            StringArray profiles{profile};
            rootNode.add(JsonNode("activeProfiles", profiles));

            response.setContent(rootNode);

            return HttpStatus::HttpOk;
        }
        return HttpStatus::HttpNotFound;
    }

    const HttpHeader Actuator::ContentTypeHeader("Content-Type",
                                                 "application/vnd.summer-boot.actuator.v2+json;charset=UTF-8");

    Actuator::Actuator() {
    }

    Actuator::~Actuator() {
    }

    bool Actuator::initialize() {
        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IConfigService *cs = factory->getService<IConfigService>();
        assert(cs);

        Port actuatorPort;
        if (cs->getProperty("management.server.port", actuatorPort)) {
            // start actuator http server.
            String serverHost;
            cs->getProperty("management.server.host", serverHost);
            HttpServer::Context context(
                    Endpoint((serverHost.isNullOrEmpty() ? String("any") : serverHost), actuatorPort));
            HttpServer::Actions actions(this, onAction);
            if (!_httpServer.startHttpServer(context, actions)) {
                Trace::writeLine("Can not start actuator http server.", Trace::Error);
                return false;
            }

            return true;
        } else {
            bool enable = true;
            if (cs->getProperty("server.enabled", enable) && !enable) {
                return false;
            }
            String scheme;
            if (!(cs->getProperty("server.scheme", scheme) && (scheme == "http" || scheme == "https"))) {
                return false;
            }
            Port serverPort;
            if (!cs->getProperty("server.port", serverPort)) {
                Trace::error("server.port is incorrect.");
                return false;
            }
            IHttpRegister *hs = factory->getService<IHttpRegister>();
            assert(hs);
            hs->registerAction(this);

            return true;
        }
    }

    bool Actuator::unInitialize() {
        _httpServer.stop();

        ServiceFactory *factory = ServiceFactory::instance();
        assert(factory);
        IHttpRegister *hs = factory->getService<IHttpRegister>();
        assert(hs);
        hs->deregisterAction(this);

        return true;
    }

    HttpStatus Actuator::onAction(const HttpRequest &request, HttpResponse &response) {
        HttpStatus status = HttpStatus::HttpNotFound;
        if (request.match("actuator")) {
            status = onActuator(request, response);
        } else if (request.match("actuator/info")) {
            status = onInfo(request, response);
        } else if (request.match("actuator/health")) {
            status = onHealth(request, response);
        } else if (request.match("actuator/metrics")) {
            status = onMetrics(request, response);
        } else if (request.match("actuator/metrics/process.uptime")) {
            status = onProcessUptime(request, response);
        } else if (request.match("actuator/metrics/system.cpu.count")) {
            status = onSystemCPUCount(request, response);
        } else if (request.match("actuator/metrics/process.cpu.usage")) {
            status = onProcessCPUUsage(request, response);
        } else if (request.match("actuator/metrics/system.cpu.usage")) {
            status = onSystemCPUUsage(request, response);
        } else if (request.match("actuator/metrics/jvm.threads.live")) {
            status = onThreadsLive(request, response);
        } else if (request.match("actuator/metrics/jvm.threads.peak")) {
            status = onThreadsPeak(request, response);
        } else if (request.match("actuator/metrics/jvm.threads.daemon")) {
            status = onThreadsDaemon(request, response);
        } else if (request.match("actuator/metrics/jvm.memory.max")) {
            status = onMemoryMax(request, response);
        } else if (request.match("actuator/metrics/jvm.memory.used")) {
            status = onMemoryUsed(request, response);
        } else if (request.match("actuator/metrics/jvm.memory.committed")) {
            status = onMemoryCommitted(request, response);
        } else if (request.match("actuator/env")) {
            status = onEnvironment(request, response);
        } else if (request.match("actuator/env/PID")) {
            status = onProcessId(request, response);
        } else if (request.match("actuator/service-registry")) {
            response.setContent("UP");
            status = HttpStatus::HttpOk;
        }

        if (status == HttpStatus::HttpOk) {
            response.headers.add(ContentTypeHeader);
        }
        return status;
    }

    HttpStatus Actuator::onAction(void *parameter, const HttpRequest &request, HttpResponse &response) {
        Actuator *service = (Actuator *) parameter;
        assert(service);
        return service->onAction(request, response);
    }
}
