add_executable(Timestamp_unittest Timestamp_unittest.cc)
target_link_libraries(Timestamp_unittest log_base)
add_test(NAME Timestamp_unittest COMMAND Timestamp_unittest)

add_executable(Date_unittest Date_unittest.cc)
target_link_libraries(Date_unittest log_base)
add_test(NAME Date_unittest COMMAND Date_unittest)

add_executable(TimeZone_unittest TimeZone_unittest.cc)
target_link_libraries(TimeZone_unittest log_base)
add_test(NAME TimeZone_unittest COMMAND TimeZone_unittest)

add_executable(Mutex_unittest Mutex_unittest.cc)
target_link_libraries(Mutex_unittest log_base)
add_test(NAME Mutex_unittest COMMAND Mutex_unittest)

add_executable(Thread_test Thread_test.cc)
target_link_libraries(Thread_test log_base)
add_test(NAME Thread_test COMMAND Thread_test)

add_executable(Thread_bench Thread_bench.cc)
target_link_libraries(Thread_bench log_base)
add_test(NAME Thread_bench COMMAND Thread_bench)

add_executable(ThreadLocal_test ThreadLocal_test.cc)
target_link_libraries(ThreadLocal_test log_base)
add_test(NAME ThreadLocal_test COMMAND ThreadLocal_test)

add_executable(Singleton_test Singleton_test.cc)
target_link_libraries(Singleton_test log_base)
add_test(NAME Singleton_test COMMAND Singleton_test)

add_executable(ThreadLocalSingleton_test ThreadLocalSingleton_test.cc)
target_link_libraries(ThreadLocalSingleton_test log_base)
add_test(NAME ThreadLocalSingleton_test COMMAND ThreadLocalSingleton_test)

add_executable(ThreadPool_test ThreadPool_test.cc)
target_link_libraries(ThreadPool_test log_base)
add_test(NAME ThreadPool_test COMMAND ThreadPool_test)

if(BOOSTTEST_LIBRARY)
add_executable(LogStream_test LogStream_test.cc)
target_link_libraries(LogStream_test log_base boost_unit_test_framework)
add_test(NAME LogStream_test COMMAND LogStream_test)
endif()

add_executable(LogStream_bench LogStream_bench.cc)
target_link_libraries(LogStream_bench log_base)

add_executable(Logging_test Logging_test.cc)
target_link_libraries(Logging_test log_base)

add_executable(LogFile_test LogFile_test.cc)
target_link_libraries(LogFile_test log_base)

add_executable(AsyncLogging_test AsyncLogging_test.cc)
target_link_libraries(AsyncLogging_test log_base)

add_executable(AsyncLogging_bench AsyncLogging_bench.cc)
target_link_libraries(AsyncLogging_bench log_base)