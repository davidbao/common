#!/bin/sh

runTest() {
	LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH ./$1
	result=$?
	if [ $result = 0 ];then
		echo -e "\033[32mThe test‘$1’ is passed.\033[0m"				# green
	else
		echo -e "\033[31mThe test‘$1’ is failed, result is $result\033[0m"	# red
	fi
	return $result
}

runTest ActionTest
runTest ArrayTest
runTest ByteArrayTest
runTest ConfigServiceTest
runTest DataTableTest
runTest DateTimeTest
runTest DictionaryTest
runTest DirectoryTest
runTest EnvironmentTest
runTest FileStreamTest
runTest FileTest
runTest FileTraceListenerTest
runTest HttpClientTest
runTest JsonNodeTest
runTest JsonTextReaderTest
runTest JsonTextWriterTest
runTest ListTest
runTest LoopListTest
runTest LoopVectorTest
runTest MapTest
runTest MappingStreamTest
runTest MathTest
runTest MemoryStreamTest
runTest MemoryTraceListenerTest
runTest MetricsTest
runTest PListTest
runTest PathTest
runTest PointTest
runTest RandomTest
runTest RectangleTest
runTest ResourcesTest
runTest SerialInfoTest
runTest SizeTest
runTest SqlSelectFilterTest
runTest SqliteClientTest
runTest StringArrayTest
runTest StringMapTest
runTest StringTest
runTest TcpClientTest
runTest ThreadTest
runTest TimeSpanTest
runTest TimeZoneTest
runTest TimerTest
runTest TypeInfoTest
runTest UuidTest
runTest ValueTypeTest
runTest VariantTest
runTest VectorTest
runTest VersionTest
runTest WStringTest
runTest XmlDocumentTest
runTest XmlTextReaderTest
runTest XmlTextWriterTest
runTest YmlNodeTest
runTest ZipTest
