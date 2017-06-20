#ifndef JUDGE_H
#define JUDGE_H

#include "stdafx.h"

#define COPY_FAILED (-1)
#define CREATE_FILE_FAILED (-2)
#define WAIT_PROGRAM_FAILED (-3)
#define GET_EXIT_CODE_FAILED (-4)
#define CREATE_PROCESS_FAILED (-5)
#define TERMINATE_PROCESS_FAILED (-6)
#define MAKE_DIR_FAILED (-7)
#define DELETE_FILE_FAILED (-8)
#define HAVE_NOT_INITED (-9)
#define TEST_SUCCESS (1)

class Judge
{
public:
	struct TEST_INFORMATION;
	typedef TEST_INFORMATION TINFO, *PTINFO;

private:
	wchar_t ini[MAX_PATH];
	HANDLE h_file;
	bool have_init;
	//通过输入文件查找输出文件
	bool search_out(LPCWSTR, LPWSTR);
	//比对文件，并输出Wrong answer信息
	bool CompareFile(LPCWSTR, LPCWSTR, LPWSTR, LPCWSTR, PTINFO);
	//获取自身程序名
	void GetMyName(LPWSTR,size_t);

protected:
	//设置信息
	wchar_t path[MAX_PATH];
	wchar_t user_program[MAX_PATH];
	wchar_t input_data_form[MAX_PATH];
	wchar_t output_data_form[MAX_PATH];
	int time_limit;
	int memory_limit;
	//检查设置是否合法
	bool check_config();
	//自动获取设置
	bool load_config();
	//直接输入设置
	bool load_config(LPCWSTR, LPCWSTR, LPCWSTR, int, int);
	//查找下一个数据文件
	bool search_data(LPWSTR, LPWSTR, size_t);
	//初始化test环境
	int init_test();
	//测试程序
	int run_test(LPCWSTR, LPCWSTR, PTINFO);
	//关闭test
	int close_test();

	Judge();
	virtual ~Judge();
};

struct Judge::TEST_INFORMATION
{
	int lsize;
	int tTime;
	long long tMemory;
	wchar_t log[2048];
	enum
	{
		ACCEPT,
		WRONG_ANSWER,
		RUNTIME_ERROR,
		TIME_LIMIT_EXCEED,
		MEMORY_LIMIT_EXCEED
	}tResult;
	TEST_INFORMATION() {}
};

#endif //JUDGE_H