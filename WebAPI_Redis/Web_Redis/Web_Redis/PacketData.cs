﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Web_Redis
{
    //Echo
    public struct REQ_DEV_ECHO
    {
        public int WaitSec;
        public string ReqData;
    }

    public struct RES_DEV_ECHO
    {
        public bool Result;
        public string ResData;
    }



    //Create User
    public struct REQ_CREATE_USER
    {
        public string UserID;
        public string PW;
    }

    public struct RES_CREATE_USER
    {
        public RES_CREATE_USER Return(ERROR_CODE error)
        {
            Result = (short)error; return this;
        }

        public void SetResult(ERROR_CODE error) { Result = (short)error; }


        public short Result;
    }

    //Login
    public struct REQ_LOGIN
    {
        public string UserID;
        public string PW;
    }

    public struct RES_LOGIN
    {
        public RES_LOGIN Return(ERROR_CODE error)
        {
            Result = (short)error; return this;
        }

        public void SetResult(ERROR_CODE error) { Result = (short)error; }


        public short Result;
        public string AuthToken;
    }

    //Load Basic Game Data
    public struct REQ_LOAD_BASIC_GAME_DATA
    {
        public string UserID;
        public string AuthToken;
    }

    public struct RES_LOAD_BASIC_GAME_DATA
    {
        public RES_LOAD_BASIC_GAME_DATA Return(ERROR_CODE error)
        {
            Result = (short)error; return this;
        }

        public void SetResult(ERROR_CODE error) { Result = (short)error; }


        public short Result;
        public int Level;
        public string Money;
    }



}
