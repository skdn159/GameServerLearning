﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Web_Redis.Data
{
    public static class AuthTokenRepository
    {
        static Dictionary<string, string> AuthTokenDic = new Dictionary<string, string>();


        public static async Task Add(string userID, string authToken) // redis와 데이터 통신을 하니까 async, await
        {
            await DB.Redis.SetStringAsync<DBUserSession>(userID, new DBUserSession() { AuthToken = authToken, CV = 1, CDV = 1 });
        }

        public static async Task<bool> Check(string userID, string authToken)
        {
            var sessionInfo = await DB.Redis.GetStringAsync<DBUserSession>(userID);
            if (sessionInfo.Item1 == false || sessionInfo.Item2.AuthToken != authToken)
            {
                return false;
            }

            return true;
        }

    }

    public struct DBUserSession
    {
        public string AuthToken;
        public short CV;            // ClientVersion
        public short CDV;           // ClientDataVersion
    }
}
