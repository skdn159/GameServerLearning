﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Web.Http;

namespace Web_Redis.Request
{
    public class LoadBasicGameDataController : ApiController
    {
        [Route("Request/LoadBasicGameData")]
        [HttpPost]
        public async Task<RES_LOAD_BASIC_GAME_DATA> Process(REQ_LOAD_BASIC_GAME_DATA requestPacket)
        {
            var responseResult = new RES_LOAD_BASIC_GAME_DATA();

            var authCheckResult = await Data.AuthTokenRepository.Check(requestPacket.UserID, requestPacket.AuthToken);

            if (authCheckResult == false)
            {
                return responseResult.Return(ERROR_CODE.REQ_LOAD_BASIC_GAME_DATA_INVALID_AUTH);
            }

//             using (var reqLock = new RequestLock(requestPacket.UserID))
//             {
//                 var error = reqLock.RequestProcessing();
//                 if (error != ERROR_CODE.NONE)
//                 {
//                     return responseResult.Return(error);
//                 }
// 
//                 var gameData = await Data.BasicGameRepository.GetGameData(requestPacket.UserID);
//                 if (gameData == null)
//                 {
//                     return responseResult.Return(ERROR_CODE.REQ_LOAD_BASIC_GAME_DATA_INVALID_ID);
//                 }
// 
//                 responseResult.SetResult(ERROR_CODE.NONE);
//                 responseResult.Level = gameData.Level;
//                 responseResult.Money = gameData.Money.ToString();
//             }


             var gameData = await Data.BasicGameRepository.GetGameData(requestPacket.UserID);
            if (gameData == null)
            {
                return responseResult.Return(ERROR_CODE.REQ_LOAD_BASIC_GAME_DATA_INVALID_ID);
            }
            
            responseResult.SetResult(ERROR_CODE.NONE);
            responseResult.Level = gameData.Level;
            responseResult.Money = gameData.Money.ToString();




                return responseResult;
        }
    }
}
