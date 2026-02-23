add_executable(Tests)

target_sources(Tests
	PRIVATE
		TestAccount.cpp
)

target_link_libraries(Tests
	PRIVATE
		Core
)

function(Tests test)
	add_test(
		NAME ${test}
		COMMAND Tests ${test}
	)
endfunction()

Tests(username)
Tests(correct_password)
Tests(wrong_password)
