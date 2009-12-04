class Test
{
   static var processed = 0;

   static public function main()
   {
      // Called in single threaded mode...
      //fastcgi.Request.init(":2288");
      fastcgi.Request.init("");
      // This can be called multi-threaded...
      var req = new fastcgi.Request();
      while( req.nextRequest() )
      {
         req.write( "Content-type: text/html\r\n" +
            "\r\n" +
            "<title>Neko FastCGI</title>" +
            "<h1>Fast CGI</h1> Requests processed here: " + (processed++) );

         req.close();
      }
   }
}
