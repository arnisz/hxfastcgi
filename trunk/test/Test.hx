class Test
{
   static public function main()
   {
      // Called in single threaded mode...
      fastcgi.Request.init(":2288");
      // This can be called multi-threaded...
      var req = new fastcgi.Request();
      if( req.nextRequest() )
      {
         req.write( "Content-type: text/html\r\n" +
            "\r\n" +
            "<title>FastCGI Hello! (multi-threaded C, fcgiapp library)</title>" +
            "<h1>FastCGI Hello! (multi-threaded C, fcgiapp library)</h1>" );

         req.close();
      }
   }
}
