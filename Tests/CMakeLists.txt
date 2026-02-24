add_executable(TestAccount)
add_executable(TestDatetime)
add_executable(TestContact)
add_executable(TestStudyPeriod)

target_sources(TestAccount PRIVATE TestAccount.cpp)
target_sources(TestDatetime PRIVATE TestDatetime.cpp)
target_sources(TestContact PRIVATE TestContact.cpp)
target_sources(TestStudyPeriod PRIVATE TestStudyPeriod.cpp)

target_link_libraries(TestAccount PRIVATE Core)
target_link_libraries(TestDatetime PRIVATE Core)
target_link_libraries(TestContact PRIVATE Core)
target_link_libraries(TestStudyPeriod PRIVATE Core)

function(TestAccount test)
	add_test(
		NAME ${test}
		COMMAND TestAccount ${test}
	)
endfunction()

function(TestDatetime test)
	add_test(
		NAME ${test}
		COMMAND TestDatetime ${test}
	)
endfunction()

function(TestContact test)
	add_test(
		NAME ${test}
		COMMAND TestContact ${test}
	)
endfunction()

function(TestStudyPeriod test)
	add_test(
		NAME ${test}
		COMMAND TestStudyPeriod ${test}
	)
endfunction()

TestAccount(username)
TestAccount(correct_password)
TestAccount(wrong_password)

TestDatetime(valid)
TestDatetime(invalid_date)
TestDatetime(invalid_time)
TestDatetime(now)
TestDatetime(add_days)
TestDatetime(add_days_month)
TestDatetime(add_days_year)
TestDatetime(days_between)
TestDatetime(compare)

TestContact(getter)
TestContact(empty)

TestStudyPeriod(period)
TestStudyPeriod(time_string)
TestStudyPeriod(thu_ngay)
TestStudyPeriod(consistency)
