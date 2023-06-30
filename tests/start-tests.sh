#!/bin/sh

runTest() {
	LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH ./$*
	result=$?
	if [ $result = 0 ];then
		echo -e "\033[32mThe test‘$1’ is passed.\033[0m"				# green
	else
		echo -e "\033[31mThe test‘$1’ is failed, result is $result\033[0m"	# red
	fi
	return $result
}

runTest IO/FileStreamTest
runTest IO/MappingStreamTest
runTest IO/MetricsTest
runTest IO/DirectoryTest
runTest IO/PathTest
runTest IO/ZipTest
runTest IO/MemoryStreamTest
runTest IO/SerialInfoTest
runTest IO/FileTest
runTest data/ArrayTest
runTest data/ValueTypeTest
runTest data/PointTest
runTest data/PListTest
runTest data/ListTest
runTest data/StringArrayTest
runTest data/StringTest
runTest data/ByteArrayTest
runTest data/TimeZoneTest
runTest data/RectangleTest
runTest data/TimeSpanTest
runTest data/StringMapTest
runTest data/UuidTest
runTest data/VersionTest
runTest data/LoopVectorTest
runTest data/DateTimeTest
runTest data/LoopListTest
runTest data/TypeInfoTest
runTest data/DictionaryTest
runTest data/WStringTest
runTest data/VariantTest
runTest data/MapTest
runTest data/SizeTest
runTest data/VectorTest
runTest json/JsonTextReaderTest
runTest json/JsonNodeTest
runTest json/JsonTextWriterTest
runTest net/NetTypeTest
runTest net/TcpClientTest
runTest crypto/RSAProviderTest
runTest crypto/SHAProviderTest
runTest crypto/AESProviderTest
runTest crypto/Md5ProviderTest
runTest crypto/DESProviderTest
runTest crypto/SmProviderTest
runTest microservice/SummerApplicationTest
runTest rpc/RpcClientTest
runTest yml/YmlNodeTest
runTest xml/XmlDocumentTest
runTest xml/XmlTextReaderTest
runTest xml/XmlTextWriterTest
runTest system/RandomTest
runTest system/ResourcesTest
runTest system/EnvironmentTest
runTest system/DelegateTest
runTest system/MathTest
runTest system/ActionTest
runTest diag/FileTraceListenerTest
runTest diag/MemoryTraceListenerTest
runTest thread/TaskTest
runTest thread/TimerTest
runTest thread/ThreadTest
runTest thread/TaskTimerTest
runTest database/SqlSelectFilterTest
runTest database/DataTableTest
runTest database/KingbaseClientTest -h=192.166.1.3
runTest database/MysqlClientTest -h=192.166.1.3
runTest database/SqliteClientTest
runTest configuration/ConfigServiceTest
runTest http/HttpContentTest
runTest http/HttpClientTest
