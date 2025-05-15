const char *webPageStart = R"Shit(
<!DOCTYPE html>
<html>
    <head>
        <title>XClock WiFi Configuration</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    
    <style type="text/css">
        .input{display: block; margin-top: 10px;}
        .input span{width: 100px; float: left; float: left; height: 36px; line-height: 36px;}
        .input input{height: 30px;width: 200px;}
        .btn{width: 120px; height: 35px; background-color: #000000; border:0px; color: #ffffff;
                margin-top:15px; margin-left:100px;}
    </style>
    
    <body>
        <form method="POST" action="configwifi">
            <label class="input">
                <span>
                    WiFi Name
                </span>
                <input type="text" name="ssid">
            </label>

            <label class="input">
                <span>
                    Password
                </span> 
                <input type="text" name="pass">
            </label>

            <input class="btn" type="submit" name="submit" value="Submit">         
        </form>
        <p>Nearby WiFi networks:</p>
        <ul>

)Shit";

const char *webPageEnd = R"Shit(
        </ul>
    </body>
</html>)Shit";

const char *noSSIDPage = "<b>Error: No SSID argument</b>";
const char *noPSKPage = "<b>Error: No password argument</b>";