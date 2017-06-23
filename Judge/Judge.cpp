#include "stdafx.h"
#include "Judge.h"

Judge::Judge()
{
	if (GetModuleFileName(NULL, path, MAX_PATH))
		wcsrchr(path, '\\')[1] = '\0';
	wcscpy_s(ini, path);
	wcscat_s(ini, TEXT("JudgeFile.ini"));
	h_file = INVALID_HANDLE_VALUE;
	time_limit = 0x7FFFFFFF;
	memory_limit = 0x7FFFFFFF;
	have_init = false;
}

Judge::~Judge()
{
}

void Judge::GetMyName(LPWSTR ret, size_t _size)
{
	wchar_t tmp[MAX_PATH];
	if (!GetModuleFileName(NULL, tmp, MAX_PATH))
	{
		ret[0] = '\0';
		return;
	}
	wcscpy_s(ret, _size, wcsrchr(tmp, '\\') + 1);
}

bool check_config(LPCWSTR str)
{
	int len;
	if ((len = wcslen(str)) == 0)
		return false;
	bool ret;
	if (wcsrchr(str, '.') == NULL)
	{
		ret = false;
		for (int i = 0; i<len; i++)
			if (str[i]>32 && str[i]<127)
			{
				ret = true; break;
			}
		if (!ret)return false;
		return true;
	}
	int dot = wcsrchr(str, '.') - str;
	ret = false;
	for (int i = dot + 1; i<len; i++)
		if (str[i]>32 && str[i]<127)
		{
			ret = true; break;
		}
	if (!ret)return false;
	ret = false;
	for (int i = 0; i<dot; i++)
		if (str[i]>32 && str[i]<127)
		{
			ret = true; break;
		}
	if (!ret)return false;
	return true;
}

bool Judge::check_config()
{
	if (!::check_config(user_program)
		|| !::check_config(input_data_form)
		|| !::check_config(output_data_form))
		return false;
	int cnt1 = 0, cnt2 = 0, l1 = wcslen(input_data_form), l2 = wcslen(output_data_form);
	for (int i = 0; i<l1; i++)cnt1 += input_data_form[i] == '*';
	for (int i = 0; i<l2; i++)cnt2 += output_data_form[i] == '*';
	if (cnt1>1 || cnt2>1 || cnt1 != cnt2)
		return false;
	if (_waccess(user_program, 0) != 0)
		return false;
	WIN32_FIND_DATA tmp;
	if (FindFirstFile(input_data_form, &tmp) == INVALID_HANDLE_VALUE)
		return false;
	if (FindFirstFile(output_data_form, &tmp) == INVALID_HANDLE_VALUE)
		return false;
	if (time_limit <= 0 || memory_limit <= 0)
		return false;
	return true;
}

bool Judge::load_config()
{
	GetPrivateProfileString(TEXT("file_config"), TEXT("user_program"), TEXT(""), user_program, MAX_PATH, ini);
	GetPrivateProfileString(TEXT("file_config"), TEXT("input_data_form"), TEXT(""), input_data_form, MAX_PATH, ini);
	GetPrivateProfileString(TEXT("file_config"), TEXT("output_data_form"), TEXT(""), output_data_form, MAX_PATH, ini);
	wchar_t s[20];
	GetPrivateProfileString(TEXT("runtime_config"), TEXT("time_limit"), TEXT("1000"), s, 20, ini);
	swscanf_s(s, TEXT("%d"), &time_limit);
	GetPrivateProfileString(TEXT("runtime_config"), TEXT("memory_limit"), TEXT("131072"), s, 20, ini);
	swscanf_s(s, TEXT("%d"), &memory_limit);
	if (wcslen(user_program) == 0)
	{
		HANDLE hTemp;
		WIN32_FIND_DATA da;
		ZeroMemory(&da, sizeof da);
		wchar_t myname[MAX_PATH];
		GetMyName(myname, MAX_PATH);
		if (myname[0] == '\0')return false;
		hTemp = FindFirstFile(TEXT("*.exe"), &da);
		while (hTemp != INVALID_HANDLE_VALUE&&wcscmp(myname, da.cFileName) == 0)
			FindNextFile(hTemp, &da);
		if (hTemp == INVALID_HANDLE_VALUE)return false;
		wcscpy_s(user_program, da.cFileName);
	}
	if (wcslen(input_data_form) == 0)
	{
		HANDLE hTemp;
		WIN32_FIND_DATA da;
		ZeroMemory(&da, sizeof da);
		hTemp = FindFirstFile(TEXT("*.in"), &da);
		if (hTemp == INVALID_HANDLE_VALUE)return false;
		wcscpy_s(input_data_form, da.cFileName);
		wchar_t *x = wcsrchr(input_data_form, '.'), *y = x;
		y--;
		while (y >= input_data_form&&*y >= '0'&&*y <= '9')
		{
			*y = '\0';
			y--;
		}
		y++;
		*y = '*';
		y++;
		wcscpy_s(y, MAX_PATH - (y - input_data_form), x);
	}
	if (wcslen(output_data_form) == 0)
	{
		HANDLE hTemp;
		WIN32_FIND_DATA da;
		ZeroMemory(&da, sizeof da);
		hTemp = FindFirstFile(TEXT("*.out"), &da);
		if (hTemp == INVALID_HANDLE_VALUE)
		{
			hTemp = FindFirstFile(TEXT("*.ans"), &da);
			if (hTemp == INVALID_HANDLE_VALUE)
				return false;
		}
		wcscpy_s(output_data_form, da.cFileName);
		wchar_t *x = wcsrchr(output_data_form, '.'), *y = x;
		y--;
		while (y >= output_data_form&&*y >= '0'&&*y <= '9')
		{
			*y = '\0';
			y--;
		}
		y++;
		*y = '*';
		y++;
		wcscpy_s(y, MAX_PATH - (y - output_data_form), x);
	}
	if (!check_config())
		return false;
	WritePrivateProfileString(TEXT("file_config"), TEXT("user_program"), user_program, ini);
	WritePrivateProfileString(TEXT("file_config"), TEXT("input_data_form"), input_data_form, ini);
	WritePrivateProfileString(TEXT("file_config"), TEXT("output_data_form"), output_data_form, ini);
	swprintf_s(s, TEXT("%d"), time_limit);
	WritePrivateProfileString(TEXT("runtime_config"), TEXT("time_limit"), s, ini);
	swprintf_s(s, TEXT("%d"), memory_limit);
	WritePrivateProfileString(TEXT("runtime_config"), TEXT("memory_limit"), s, ini);
	return true;
}

bool Judge::load_config(LPCWSTR _user_program, LPCWSTR _input_data_form, LPCWSTR _output_data_form, int tl, int ml)
{
	wcscpy_s(user_program, _user_program);
	wcscpy_s(input_data_form, _input_data_form);
	wcscpy_s(output_data_form, _output_data_form);
	time_limit = tl;
	memory_limit = ml;
	if (!check_config())
		return false;
	WritePrivateProfileString(TEXT("file_config"), TEXT("user_program"), user_program, ini);
	WritePrivateProfileString(TEXT("file_config"), TEXT("input_data_form"), input_data_form, ini);
	WritePrivateProfileString(TEXT("file_config"), TEXT("output_data_form"), output_data_form, ini);
	wchar_t s[20];
	swprintf_s(s, TEXT("%d"), time_limit);
	WritePrivateProfileString(TEXT("runtime_config"), TEXT("time_limit"), s, ini);
	swprintf_s(s, TEXT("%d"), memory_limit);
	WritePrivateProfileString(TEXT("runtime_config"), TEXT("memory_limit"), s, ini);
	return true;
}

bool Judge::set_path(LPWSTR stpath)
{
	if (!SetCurrentDirectory(stpath))
		return false;
	int len = wcslen(stpath);
	if (stpath[len - 1] == '\n')
		stpath[--len] = 0;
	if (stpath[len - 1] != '\\')
		stpath[len++] = '\\';
	wcscpy_s(path, stpath);
	wcscpy_s(ini, path);
	wcscat_s(ini, TEXT("JudgeFile.ini"));
	return true;
}

bool Judge::search_out(LPCWSTR in, LPWSTR out)
{
	int st = wcschr(input_data_form, '*') - input_data_form;
	int ed = wcslen(in) - (wcslen(input_data_form) - st - 1);
	int i = 0, k;
	for (; output_data_form[i] != '*'; i++)
		out[i] = output_data_form[i];
	k = i + 1;
	for (int j = st; j<ed; j++)
		out[i++] = in[j];
	int l = wcslen(output_data_form);
	for (; k <= l; k++)
		out[i++] = output_data_form[k];
	out[i++] = '\0';
	WIN32_FIND_DATA p;
	if (FindFirstFile(out, &p) == INVALID_HANDLE_VALUE)
		return false;
	return true;
}

bool Judge::search_data(LPWSTR in_data, LPWSTR out_data, size_t _size)
{
	WIN32_FIND_DATA in;
	if (h_file == INVALID_HANDLE_VALUE)
	{
		h_file = FindFirstFile(input_data_form, &in);
		while (!search_out(in.cFileName, out_data))
			if (!FindNextFile(h_file, &in))
				return false;
		wcscpy_s(in_data, _size, in.cFileName);
		return true;
	}
	do
		if (!FindNextFile(h_file, &in))
			return false;
	while (!search_out(in.cFileName, out_data));
	wcscpy_s(in_data, _size, in.cFileName);
	return true;
}

bool Judge::CompareFile(LPCWSTR file1, LPCWSTR file2, LPWSTR log, LPCWSTR _end, PTINFO result)
{
	FILE *f1, *f2;
	_wfopen_s(&f1, file1, TEXT("r"));
	_wfopen_s(&f2, file2, TEXT("r"));
	wchar_t s1[1024], s2[1024], *r1, *r2;
	bool flag = 1;
	do
	{
		r1 = fgetws(s1, 1024, f1);
		r2 = fgetws(s2, 1024, f2);
		if ((r1 == NULL&&r2 != NULL) || (r1 != NULL&&r2 == NULL))
		{
			flag = 0; break;
		}
		if (r1 == NULL&&r2 == NULL)break;
		if (wcscmp(s1, s2) != 0)
		{
			flag = 0; break;
		}
	} while (1);
	fclose(f1);
	fclose(f2);
	if (!flag)
	{
		log += swprintf_s(log, _end - log, TEXT("Wrong answer![time:%5dms] [memory:%6I64dKB]\n\n"), result->tTime, result->tMemory);
		int i;
		for (i = 0; i<1024; i++)
			if (s1[i] != s2[i])
			{
				i = i >= 10 ? i - 10 : 0;
				int j = i + 50<1024 ? i + 50 : 1023;
				s1[j] = 0; s2[j] = 0;
				log += swprintf_s(log, _end - log, TEXT("expect: %s\n"), s1 + i);
				log += swprintf_s(log, _end - log, TEXT("yours : %s\n"), s2 + i);
				break;
			}
	}
	return flag;
}

int Judge::init_test()
{
	//创建临时文件夹
	if (_waccess(TEXT("JudgeTemp"), 0) == -1)
		if (_mkdir("JudgeTemp") == -1)
			return MAKE_DIR_FAILED;
	//复制文件
	if (!CopyFile(user_program, TEXT(".\\JudgeTemp\\prog.exe"), false))
		return COPY_FAILED;
	have_init = true;
	return TEST_SUCCESS;
}

int Judge::run_test(LPCWSTR in, LPCWSTR out, PTINFO result)
{
	if (!have_init)
		return HAVE_NOT_INITED;
	//复制文件
	if (!CopyFile(in, TEXT(".\\JudgeTemp\\data.in"), false))
		return COPY_FAILED;
	if (!CopyFile(out, TEXT(".\\JudgeTemp\\data.out"), false))
		return COPY_FAILED;
	//设置安全信息
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = true;
	//创建文件句柄
	HANDLE hIn = new HANDLE, hOut = new HANDLE;
	hIn = CreateFile(TEXT(".\\JudgeTemp\\data.in"), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hIn == INVALID_HANDLE_VALUE)return CREATE_FILE_FAILED;
	hOut = CreateFile(TEXT(".\\JudgeTemp\\prog_out.out"), GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, &sa, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOut == INVALID_HANDLE_VALUE)return CREATE_FILE_FAILED;
	//设置进程信息
	STARTUPINFO si;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	GetStartupInfo(&si);
	si.cb = sizeof(STARTUPINFO);
	si.hStdInput = hIn;
	si.hStdOutput = hOut;
	si.hStdError = hOut;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	si.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof pi);
	ZeroMemory(result, sizeof(TINFO));
	result->lsize = 2048;
	//日志输出
	wchar_t *prc = result->log, *p_end = result->log + result->lsize - 1;
	prc += swprintf_s(prc, p_end-prc, TEXT("{%s %s}\n"), in, out);
	//运行进程
	SetErrorMode(SEM_NOGPFAULTERRORBOX);
	if (CreateProcess(TEXT(".\\JudgeTemp\\prog.exe"), NULL, NULL, NULL, true, CREATE_SUSPENDED, NULL, TEXT(".\\JudgeTemp"), &si, &pi))
	{
		int exitcode, temp;
		ResumeThread(pi.hThread);
		//获取运行时间
		clock_t st = clock();
		temp = WaitForSingleObject(pi.hProcess, time_limit);
		clock_t ed = clock();
		if (result)result->tTime = (int)(((double)(ed - st) / CLOCKS_PER_SEC) * 1000);
		//获取运行内存
		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(pi.hProcess, &pmc, sizeof pmc);
		result->tMemory = pmc.PeakPagefileUsage / 1024;
		//判断测试结果
		switch (temp)
		{
		case WAIT_OBJECT_0:
			if (result->tMemory>memory_limit)
				result->tResult = TINFO::MEMORY_LIMIT_EXCEED;
			break;
		case WAIT_TIMEOUT:
			result->tResult = TINFO::TIME_LIMIT_EXCEED;
			break;
		default:
			return WAIT_PROGRAM_FAILED;
		}
		if (GetExitCodeProcess(pi.hProcess, (DWORD*)&exitcode))
		{
			if (exitcode != 0 && result->tResult != TINFO::TIME_LIMIT_EXCEED)
				result->tResult = TINFO::RUNTIME_ERROR;
		}
		else
			return GET_EXIT_CODE_FAILED;
	}
	else
		return CREATE_PROCESS_FAILED;
	CloseHandle(hIn);
	CloseHandle(hOut);
	if (result->tResult == TINFO::TIME_LIMIT_EXCEED)
		if (!TerminateProcess(pi.hProcess, 0))
			return TERMINATE_PROCESS_FAILED;
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	SetErrorMode(0);
	if (result->tResult != TINFO::MEMORY_LIMIT_EXCEED&&
		result->tResult != TINFO::TIME_LIMIT_EXCEED&&
		result->tResult != TINFO::RUNTIME_ERROR &&
		!CompareFile(TEXT(".\\JudgeTemp\\data.out"), TEXT(".\\JudgeTemp\\prog_out.out"), prc, p_end, result))
		result->tResult = TINFO::WRONG_ANSWER;
	switch (result->tResult)
	{
	case TINFO::ACCEPT:
		prc += swprintf_s(prc, p_end-prc, TEXT("Accept![time:%5dms] [memory:%6I64dKB]\n"), result->tTime, result->tMemory);
		break;
	case TINFO::RUNTIME_ERROR:
		prc += swprintf_s(prc, p_end-prc, TEXT("Runtime error!\n"));
		break;
	case TINFO::TIME_LIMIT_EXCEED:
		prc += swprintf_s(prc, p_end - prc, TEXT("Time limit exceed!\n"));
		break;
	case TINFO::MEMORY_LIMIT_EXCEED:
		prc += swprintf_s(prc, p_end - prc, TEXT("Memory limit exceed![memory:%6I64dKB]\n"), result->tMemory);
		break;
	default:
		return false;
	}
	prc += swprintf_s(prc, p_end - prc, TEXT("\n"));
	return TEST_SUCCESS;
}

int Judge::close_test()
{
	have_init = false;
	if (h_file != INVALID_HANDLE_VALUE)
		FindClose(h_file);
	if (_waccess(TEXT("JudgeTemp"), 0) == 0)
	{
		if (DeleteFile(TEXT(".\\JudgeTemp\\data.in")) == 0)
			return DELETE_FILE_FAILED;
		if (DeleteFile(TEXT(".\\JudgeTemp\\data.out")) == 0)
			return DELETE_FILE_FAILED;
		if (DeleteFile(TEXT(".\\JudgeTemp\\prog.exe")) == 0)
			return DELETE_FILE_FAILED;
		if (DeleteFile(TEXT(".\\JudgeTemp\\prog_out.out")) == 0)
			return DELETE_FILE_FAILED;
		if (_rmdir("JudgeTemp") == -1)
			return DELETE_FILE_FAILED;
	}
	return TEST_SUCCESS;
}
