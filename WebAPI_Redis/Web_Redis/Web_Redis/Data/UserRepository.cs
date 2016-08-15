using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using MongoDB.Driver;

namespace Web_Redis.Data
{
    public static class UserRepository
    {
        public static async Task AddUser(string userID, string pw)
        {
            var user = new DB.DBUser()
            {
                UID = DateTime.Now.Ticks,  // 원래 snowFlake를 사용해서 만들어 넣어야함
                _id = userID,
                PW = pw,
            };

            var collection = DB.MongoDBLib.GetAccountDBUserCollection<DB.DBUser>();
            await collection.InsertOneAsync(user);
        }

        public static async Task<DB.DBUser> GetUser(string userID)
        {
            var collection = DB.MongoDBLib.GetAccountDBUserCollection<DB.DBUser>();
            var data = await collection.Find(x => x._id == userID).FirstOrDefaultAsync();
            return data;
        }
    }
}
