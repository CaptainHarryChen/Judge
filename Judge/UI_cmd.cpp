#include "stdafx.h"
#include "UI_cmd.h"

UI_cmd::UI_cmd()
{
}

UI_cmd::~UI_cmd()
{
}

int UI_cmd::Run_Judge()
{
    wprintf(TEXT("这是一个用于测试程序的Judge。\n请将本程序和待测试程序放在测试数据文件夹里。\n温馨提示：待测试程序不需要文件输入输出。\n\n"));

    if(!query_config())
    {
        wprintf(TEXT("设置读取失败！\n"));
        return CONFIG_READ_FAILED;
    }

    if(!Judge_program())
        return JUDGE_FAILED;
    return JUDGE_SUCCESS;
}

bool UI_cmd::query_config()
{
	int auto_input = -1;
	if (load_config())
	{
		wprintf(TEXT("是否使用以下设置(Y/N)：\n1.待测程序：%s\n2.输入文件：%s\n3.输出文件：%s\n4.时间限制：%dms\n5.内存限制：%dKB\n"), user_program, input_data_form, output_data_form, time_limit, memory_limit);
		wchar_t c = _getch();
		while (c != 'Y'&&c != 'N'&&c != 'y'&&c != 'n')
		{
			wprintf(TEXT("输入有误，请重新输入(Y/N)："));
			c = _getch();
		}
		wprintf(TEXT("\n"));
		if (c == 'Y' || c == 'y')
			auto_input = 1;
		else
			auto_input = 0;
	}
	if (auto_input == -1)
	{
		wchar_t _user_program[MAX_PATH], _input_data_form[MAX_PATH], _output_data_form[MAX_PATH], s[20];
		int _tl, _ml;
		wprintf(TEXT("请输入你的exe文件名："));
		fgetws(_user_program, MAX_PATH, stdin);
		int len = wcslen(_user_program) - 1;
		if (_user_program[len] == '\n')_user_program[len] = '\0';
		wprintf(TEXT("请输入输入数据文件名（*为通配符，只允许有一个通配符）："));
		fgetws(_input_data_form, MAX_PATH, stdin);
		len = wcslen(_input_data_form) - 1;
		if (_input_data_form[len] == '\n')_input_data_form[len] = '\0';
		wprintf(TEXT("请输入输出数据文件名（*为通配符，只允许有一个通配符）："));
		fgetws(_output_data_form, MAX_PATH, stdin);
		len = wcslen(_output_data_form) - 1;
		if (_output_data_form[len] == '\n')_output_data_form[len] = '\0';
		wprintf(TEXT("请输入时间限制(ms)："));
		fgetws(s, 20, stdin);
		swscanf_s(s, TEXT("%d"), &_tl);
		wprintf(TEXT("请输入内存限制(MB)："));
		fgetws(s, 20, stdin);
		swscanf_s(s, TEXT("%d"), &_ml);
		_ml *= 1024;
		if (!load_config(_user_program, _input_data_form, _output_data_form, _tl, _ml))
			return false;
		wprintf(TEXT("\n"));
	}
	else if (auto_input == 0)
	{
		while(1)
		{
			wprintf(TEXT("输入要更改的设置的编号："));
			int id;
			while (1)
			{
				wchar_t input[10];
				fgetws(input, 10, stdin);
				int len = wcslen(input);
				if (input[len - 1] == '\n')
					input[--len] = 0;
				bool suc = true;
				for (int i = 0; i < len; i++)
					if (isxdigit(input[i]))
					{
						wprintf(TEXT("输入有误，请重新输入："));
						suc = false;
						break;
					}
				if (suc)
				{
					if(wcscmp(input,'Y')
				}
			}
			
	}
    return true;
}

bool UI_cmd::Judge_program()
{
    wchar_t in[MAX_PATH],out[MAX_PATH];
    TINFO ti;
    HANDLE houtput=GetStdHandle(STD_OUTPUT_HANDLE);
	FILE *olog;
	_wfopen_s(&olog,TEXT("Judge_result.log"), TEXT("w"));
    Judge::init_test();
    while(Judge::search_data(in,out,MAX_PATH))
    {
        Judge::run_test(in,out,&ti);
        SetConsoleTextAttribute(houtput,FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE);
        wprintf(TEXT("{%s %s}\n"),in,out);
        switch(ti.tResult)
        {
        case TINFO::ACCEPT:
            SetConsoleTextAttribute(houtput,FOREGROUND_INTENSITY|FOREGROUND_GREEN);
            wprintf(TEXT("Accept!             [time:%5dms] [memory:%6I64dKB]\n"),ti.tTime,ti.tMemory);
            break;
        case TINFO::WRONG_ANSWER:
            SetConsoleTextAttribute(houtput,FOREGROUND_INTENSITY|FOREGROUND_RED);
            wprintf(TEXT("Wrong answer!       [time:%5dms] [memory:%6I64dKB]\n"),ti.tTime,ti.tMemory);
            break;
        case TINFO::RUNTIME_ERROR:
            SetConsoleTextAttribute(houtput,FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_RED);
            wprintf(TEXT("Runtime error!      \n"));
            break;
        case TINFO::TIME_LIMIT_EXCEED:
            SetConsoleTextAttribute(houtput,FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
            wprintf(TEXT("Time limit exceed!  \n"));
            break;
        case TINFO::MEMORY_LIMIT_EXCEED:
            SetConsoleTextAttribute(houtput,FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
            wprintf(TEXT("Memory limit exceed![time:!!!!!ms] [memory:%6I64dKB]\n"),ti.tMemory);
            break;
        default:
            return false;
        }
        wprintf(TEXT("\n"));
        fwprintf(olog, TEXT("%s"),ti.log);
    }
    SetConsoleTextAttribute(houtput,FOREGROUND_INTENSITY);
    Judge::close_test();
    fclose(olog);
    system("notepad Judge_result.log");
    return true;
}
