﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Owin;
using System.Web.Http;

namespace Web_Redis
{
    class Startup
    {
        public void Configuration(IAppBuilder appBuilder)
        {
            var config = new HttpConfiguration();

            config.MapHttpAttributeRoutes();


//             config.Routes.MapHttpRoute(
//                 name: "DefaultApi",
//                 routeTemplate: "api/{controller}/{id}",
//                 
//                 );

            appBuilder.UseWebApi(config);


        }

    }
}
