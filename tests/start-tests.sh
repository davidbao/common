#!/bin/sh

runTest() {
	LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH ./$1
	if [ $? = 0 ];then
		echo -e "\033[32mThe test‘$1’ is passed.\033[0m"				# green
	else
		echo -e "\033[31mThe test‘$1’ is failed, result is $?\033[0m"	# red
	fi
	return $?
}
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
runTest KingbaseClientTest
runTest MapTest
runTest MappingStreamTest
runTest Md5ProviderTest
runTest MemoryStreamTest
runTest MetricsTest
runTest MysqlClientTest
runTest NetTypeTest
runTest PListTest
runTest PathTest
runTest PointTest
runTest RandomTest
runTest RectangleTest
runTest ResourcesTest
runTest SerialInfoTest
runTest SizeTest
runTest SmProviderTest
runTest SqlSelectFilterTest
runTest SqliteClientTest
runTest StringArrayTest
runTest StringMapTest
runTest StringTest
runTest SummerApplicationTest
runTest TcpClientTest
runTest TimeSpanTest
runTest TimeZoneTest
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
