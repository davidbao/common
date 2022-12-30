#!/bin/sh

runTest() {
	LD_LIBRARY_PATH=./ ./$1
	if [ $? == 0 ];then
		echo -e "\033[32mThe test‘$1’ is passed.\033[0m"				# green
	else
		echo -e "\033[31mThe test‘$1’ is failed, result is $?\033[0m"	# red
	fi
	return $?
}

runTest ByteArrayTest
runTest ConfigServiceTest
runTest DictionaryTest
runTest DirectoryTest
runTest EnvironmentTest
runTest FileTest
runTest FileTraceListenerTest
runTest HttpClientTest
runTest JsonNodeTest
runTest ListTest
runTest LoopListTest
runTest LoopVectorTest
runTest MapTest
runTest MappingStreamTest
runTest Md5ProviderTest
runTest PListTest
runTest PointTest
runTest RandomTest
runTest ResourcesTest
runTest SizeTest
runTest SmProviderTest
runTest StringArrayTest
runTest StringMapTest
runTest StringTest
runTest SummerApplicationTest
runTest TypeInfoTest
runTest UrlTest
runTest UuidTest
runTest ValueTypeTest
runTest VectorTest
runTest VersionTest
runTest WStringTest

