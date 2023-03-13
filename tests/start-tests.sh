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

runTest configuration/ConfigServiceTest
runTest crypto/Md5ProviderTest
runTest crypto/SmProviderTest
runTest data/ArrayTest
runTest data/ByteArrayTest
runTest data/DateTimeTest
runTest data/DictionaryTest
runTest data/ListTest
runTest data/LoopListTest
runTest data/LoopVectorTest
runTest data/MapTest
runTest data/PListTest
runTest data/PointTest
runTest data/RectangleTest
runTest data/SizeTest
runTest data/StringArrayTest
runTest data/StringMapTest
runTest data/StringTest
runTest data/TimeSpanTest
runTest data/TimeZoneTest
runTest data/TypeInfoTest
runTest data/UuidTest
runTest data/ValueTypeTest
runTest data/VariantTest
runTest data/VectorTest
runTest data/VersionTest
runTest data/WStringTest
runTest database/DataTableTest
runTest database/KingbaseClientTest  -h=192.166.1.19
runTest database/MysqlClientTest -h=192.166.1.16
runTest database/SqliteClientTest
runTest database/SqlSelectFilterTest
runTest diag/FileTraceListenerTest
runTest diag/MemoryTraceListenerTest
runTest http/HttpClientTest
runTest IO/DirectoryTest
runTest IO/FileStreamTest
runTest IO/FileTest
runTest IO/MappingStreamTest
runTest IO/MemoryStreamTest
runTest IO/MetricsTest
runTest IO/PathTest
runTest IO/SerialInfoTest
runTest IO/ZipTest
runTest json/JsonNodeTest
runTest json/JsonTextReaderTest
runTest json/JsonTextWriterTest
runTest microservice/SummerApplicationTest
runTest net/TcpClientTest
runTest system/ActionTest
runTest system/DelegateTest
runTest system/EnvironmentTest
runTest system/MathTest
runTest system/RandomTest
runTest system/ResourcesTest
runTest thread/TaskTest
runTest thread/TaskTimerTest
runTest thread/ThreadTest
runTest thread/TimerTest
runTest xml/XmlDocumentTest
runTest xml/XmlTextReaderTest
runTest xml/XmlTextWriterTest
runTest yml/YmlNodeTest
