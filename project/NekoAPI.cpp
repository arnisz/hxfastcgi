#define IMPLEMENT_API

#include <hxCFFI.h>
#include <fastcgi.h>
#include <fcgiapp.h>

DECLARE_KIND( k_handler );
DEFINE_KIND( k_handler );

static int sSocket = 0;

class FCGIHandler
{
public:
   FCGIHandler()
   {
       FCGX_InitRequest(&mRequest, sSocket, 0);
       mRequestOpen = false;
   }
   ~FCGIHandler()
   {
       FCGX_Free(&mRequest,1);
   }
   void Close()
   {
      if (mRequestOpen)
      {
         FCGX_Finish_r(&mRequest);
         mRequestOpen = false;
      }
   }

   int Open()
   {
      Close();
   #ifdef HAVE_PTHREAD
      static pthread_mutex_t accept_mutex = PTHREAD_MUTEX_INITIALIZER;

      /* Some platforms require accept() serialization, some don't.. */
      pthread_mutex_lock(&accept_mutex);
      int rc = FCGX_Accept_r(&mRequest);
      pthread_mutex_unlock(&accept_mutex);
   #else
      int rc = FCGX_Accept_r(&mRequest);
   #endif

      if (rc>=0)
      {
        mRequestOpen = true;
      }
      return rc;
   }

   bool Write(const char *inPtr, int inN)
   {
      return  FCGX_PutStr( inPtr, inN, mRequest.out) == inN;
   }

   char *GetParam(const char *inName)
   {
      return FCGX_GetParam(inName,mRequest.envp);
   }

   bool         mRequestOpen;
   FCGX_Request mRequest;
};


// --- Neko/C++ API -------------------------------------------------------------------

bool sgInit = false;

value fcgiInit(value inSocket)
{
   if (!sgInit)
   {
      FCGX_Init();
      sSocket = FCGX_OpenSocket( val_string(inSocket), 5 /* backlog */);
   }

   return val_null;
}

value fcgiGetNextRequest(value inHandler)
{
   if (val_is_kind(inHandler,k_handler))
   {
      FCGIHandler *handler = (FCGIHandler *)val_data(inHandler);

      int rc = handler->Open();

      return alloc_int(rc);
   }

   return val_null;
}

value fcgiCloseRequest(value inHandler)
{
   if (val_is_kind(inHandler,k_handler))
   {
      FCGIHandler *handler = (FCGIHandler *)val_data(inHandler);

      handler->Close();
   }

   return val_null;
}

value fcgiGetParam(value inHandler,value inName)
{
   if (val_is_kind(inHandler,k_handler))
   {
      FCGIHandler *handler = (FCGIHandler *)val_data(inHandler);

      char *result = handler->GetParam( val_string(inName) );

      if (!result)
         return val_null;

      return alloc_string(result);
   }

   return val_null;

}

value fcgiWrite(value inHandler,value inString)
{
   if (val_is_kind(inHandler,k_handler))
   {
      FCGIHandler *handler = (FCGIHandler *)val_data(inHandler);
      const char *ptr = val_string(inString);
      if (ptr)
         handler->Write(ptr,strlen(ptr));
   }
   return val_null;
}


// USe same signature as socket...
value fcgiWriteBytes(value inHandler,value inData,value inPos,value inLen)
{
   if (val_is_kind(inHandler,k_handler))
   {
      FCGIHandler *handler = (FCGIHandler *)val_data(inHandler);

      buffer buf = val_to_buffer(inData);
      int    pos = val_int(inPos);
      int    len = val_int(inLen);
      if (!buf)
         hx_failure("not bytebuffer");

      return alloc_bool( handler->Write( buffer_data(buf) + pos, len ));
   }

   return val_null;

}



void delete_handler( value inHandler )
{
   if ( val_is_kind( inHandler, k_handler ) )
   {
      FCGIHandler *handler = (FCGIHandler *)val_data(inHandler);
      delete handler;
   }
}


value fcgiCreateHandler()
{
   FCGIHandler *handler = new FCGIHandler();
   value v = alloc_abstract( k_handler, handler );
   val_gc( v, delete_handler );
   return v;
}

DEFINE_PRIM(fcgiInit,1);
DEFINE_PRIM(fcgiCreateHandler,0);
DEFINE_PRIM(fcgiGetNextRequest,1);
DEFINE_PRIM(fcgiGetParam,2);
DEFINE_PRIM(fcgiWrite,2);
DEFINE_PRIM(fcgiCloseRequest,1);

