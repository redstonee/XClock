const char webPage[] = R"Shit( <!DOCTYPE html>
<html>
    <head>
        <title>WIFI
            
        </title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    
    <style type="text/css">
        .input{display: block; margin-top: 10px;}
        .input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}
        .input input{height: 30px;width: 200px;}
        .btn{width: 120px; height: 35px; background-color: #000000; border:0px; color:#ffffff; margin-top:15px; margin-left:100px;}
    </style>
    
    <body>
        <form method="POST" action="configwifi">
            <label class="input">
                <span>
                    WiFi SSID  
                </span>
                <input type="text" name="ssid">
            </label>
            <label class="input">
                <span>
                    WiFi PASS
                </span> 
                <input type="text"  name="pass">
            </label>
            <input class="btn" type="submit" name="submit" value="Submie">         
            <p>
                <span> Nearby wifi:
            </P>
        </form>
    </body>
</html>)Shit";