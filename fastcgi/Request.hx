package fastcgi;

class Request
{
   var mHandler:Dynamic;

   public function new()
   {
      mHandler = fcgiCreateHandler();
   }

   public function nextRequest() : Bool
   {
      var result = fcgiGetNextRequest(mHandler);
      if (result==null)
         throw "Invalid Handle\n";
      var rc : Int = result;
      return rc == 0;
   }

   public function getParam(inName:String) : String
   {
      return fcgiGetParam(mHandler,inName);
   }

   public function write(inString:String) : String
   {
      return fcgiWrite(mHandler,inString);
   }

   public function close() : String
   {
      return fcgiCloseRequest(mHandler);
   }


   static public function init(socket:String)
   {
      fcgiInit(socket);
   }



   static var fcgiInit = neko.Lib.load("fastcgi","fcgiInit",1);
   static var fcgiCreateHandler = neko.Lib.load("fastcgi","fcgiCreateHandler",0);
   static var fcgiGetNextRequest = neko.Lib.load("fastcgi","fcgiGetNextRequest",1);
   static var fcgiGetParam = neko.Lib.load("fastcgi","fcgiGetParam",2);
   static var fcgiWrite = neko.Lib.load("fastcgi","fcgiWrite",2);
   static var fcgiCloseRequest = neko.Lib.load("fastcgi","fcgiCloseRequest",1);
}
