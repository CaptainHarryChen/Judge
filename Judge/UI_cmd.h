#ifndef UI_CMD_H
#define UI_CMD_H

#include "stdafx.h"
#include "Judge.h"

#define JUDGE_SUCCESS 0
#define CONFIG_READ_FAILED (-1)
#define JUDGE_FAILED (-2)

class UI_cmd : public Judge
{
    public:
        UI_cmd();
        virtual ~UI_cmd();
        //Ö´ÐÐJudge
        int Run_Judge();

    protected:

    private:
        bool query_config();
        bool Judge_program();
};

#endif //UI_CMD_H