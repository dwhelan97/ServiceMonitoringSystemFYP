String homePage = R"###(
<!DOCTYPE html>
<html>
<body>
  <div style="border: 1px solid #000; width: 810px; margin: 5px;">
    <div>
      <canvas id="hdrCanvas" width="800px" height="40px" style="background-color: #f0fff0; margin: 5px 5px 5px 5px;"></canvas>
    </div>
    <div style="position: relative; width: 810px; height: 610px;">
      <canvas id="myCanvasBg" style="width: 800px; height: 600px; background-color: #ffffff; position: absolute; top: 5px; left: 5px; border: 1px solid #0f0;"></canvas>
      <canvas id="myCanvas" style="width: 800px; height: 600px; position: absolute; top: 5px; left: 5px;"></canvas>
    </div>
    <div width="800px" height="40px" style="background-color: #f1f1f1; margin: 5px 5px 5px 5px;">
      <div style="width: 33%; padding: 5px; background-color: #f1f1f1;">
        <p style="text-align: center; width: 100%; padding: 0px; margin: 0px; background-color: #f1f1f1;">Time Base</p>
        <form>
          <select id="timeBase" name="timeBase" onchange="changeTimebase()" style="width: 100%; padding: 4px 4px; border: 2px solid #0ff; border-radius: 10px; background-color: #f1f1f1;">
            <option value="1">1 pixel per sample</option>
            <option value="2">2 pixels per sample</option>
            <option value="5">5 pixels per sample</option>
            <option value="10">10 pixels per sample</option>
            <option value="20">20 pixels per sample</option>
            <option value="50">50 pixels per sample</option>
            <option value="100">100 pixels per sample</option>
            <option value="200">200 pixels per sample</option>
            <option value="500">500 pixels per sample</option>
          </select>
        </form>
      </div>
      <div style="width: 33%; padding: 5px; background-color: #f1f1f1;">
        <p style="text-align: center; width: 100%; padding: 0px; margin: 0px; background-color: #f1f1f1;">Volts/Div</p>
        <form>
          <select id="voltsPerDiv" name="voltsPerDiv" onchange="changeVoltsPerDiv()" style="width: 100%; padding: 4px 4px; border: 2px solid #0ff; border-radius: 10px; background-color: #f1f1f1;">
            <option value="2.0">2.0 Volts per division</option>
            <option value="0.1">0.1 Volts per division</option>
            <option value="0.2">0.2 Volts per division</option>
            <option value="0.5">0.5 Volts per division</option>
            <option value="1.0">1.0 Volts per division</option>
            <option value="5.0">5.0 Volts per division</option>
          </select>
        </form>
      </div>
      <div style="width: 33%; padding: 5px; background-color: #f1f1f1;">
        <p style="text-align: center; width: 100%; padding: 0px; margin: 0px; background-color: #f1f1f1;">Refresh Delay</p>
        <form>
          <select id="timerDelay" name="timerDelay" onchange="changeTimerDelay()" style="width: 100%; padding: 4px 4px; border: 2px solid #0ff; border-radius: 10px; background-color: #f1f1f1;">
            <option value="1">1 millisecond</option>
            <option value="2">2 milliseconds</option>
            <option value="5">5 milliseconds</option>
            <option value="10">10 milliseconds</option>
            <option value="20">20 milliseconds</option>
            <option value="50">50 milliseconds</option>
            <option value="100">100 milliseconds</option>
            <option value="200">200 milliseconds</option>
            <option value="500">500 milliseconds</option>
            <option value="1000">1 second</option>
            <option value="2000">2 seconds</option>
            <option value="5000">5 seconds</option>
            <option value="10000">10 seconds</option>
            <option value="20000">20 seconds</option>
            <option value="50000">50 seconds</option>
          </select>
        </form>
      </div>
      <div style="width: 33%; padding: 5px; background-color: #f1f1f1;">
        <p id="frameTime" style="text-align: center; width: 100%; padding: 0px; margin: 0px; background-color: #f1f1f1;">Frame time duration: Not set</p>
      </div>
    </div>
  </div>
<script>
  var scopeWidth = 800;
  var scopeHeight = 600;
  var ampScale = (8.0*scopeHeight)/(10.0*256);
  var timeScale = 1;
  var timerDelay = 1;

  var hbc = document.getElementById("hdrCanvas");
  var hbctx = hbc.getContext("2d");
  hbctx.lineWidth = 1;
  hbctx.font = "30px Arial";
  hbctx.strokeStyle = "#000";
  hbctx.textAlign = "center";
  hbctx.strokeText("Laminarity Test Oscilliscope", hbc.width/2, 30);

  var bc = document.getElementById("myCanvasBg");
  bc.setAttribute('width', scopeWidth);
  bc.setAttribute('height', scopeHeight);
  var bctx = bc.getContext("2d");
  bctx.strokeStyle = "#a0ffa0";
  bctx.lineWidth = 1;
  // Draw y grid
  for(var ii = 0; ii < scopeWidth; ii+= scopeWidth/10){
    bctx.moveTo(ii, 0);
    bctx.lineTo(ii, scopeHeight);
    bctx.stroke();
  }
  // Draw x grid
  for(var ii = 0; ii < scopeHeight; ii+= scopeHeight/10){
    bctx.moveTo(0, ii);
    bctx.lineTo(scopeWidth, ii);
    bctx.stroke();
  }

  var c = document.getElementById("myCanvas");
  c.setAttribute('width', scopeWidth);
  c.setAttribute('height', scopeHeight);
  var ctx = c.getContext("2d");
  ctx.strokeStyle = "#000000";
  ctx.lineWidth = 2;
  var frameSize = scopeHeight;
  var orign = [10, 9*scopeHeight/10];
  var myVar = setTimeout(refreshDisplay, 10);
  var oldFrameTime = 0
  var newFrameTime = 0;
  var frameDurationMillis = "Not set yet";

  function refreshDisplay(){
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        dataFrame = convertToByteArray(this.responseText);
        plotFrame(dataFrame);
        myVar = setTimeout(refreshDisplay, timerDelay);
        newFrameTime = new Date().getTime();
        if((newFrameTime - oldFrameTime) >= 1000){
          var timeBaseText = "Time base (millis per div): " + (frameDurationMillis * 100.0) / 1000.0;
          timeBaseText += " (" + frameDurationMillis + " total screen time in missiseconds)";
          document.getElementById("frameTime").innerHTML = timeBaseText;
          oldFrameTime = newFrameTime;
        }
      } 
    };
    xhttp.open("GET", "OGDF", true); // Oscilliscope Get Data Frame
    xhttp.send();
  }
  function plotFrame(dataFrame){
    ctx.clearRect(0, 0, scopeWidth, scopeHeight);
    ctx.beginPath();
    ctx.moveTo(orign[0], orign[1]);
    for(var ii = 0; ii < dataFrame.length; ii++){
      if(ii < dataFrame.length - 1){
        ctx.lineTo(orign[0] + ii*timeScale, orign[1]-dataFrame[ii]);
      }
    }
    ctx.stroke();
  }
  function convertToByteArray(ajaxText){
  //  frameDurationMillis = ajaxText.charAt[ajaxText.length - 2]*256 + ajaxText.charAt[ajaxText.length - 1];
    frameDurationMillis = parseInt("0x" +ajaxText.substring(ajaxText.length-2, ajaxText.length));
    var dataFrame = [];
    for(var ii = 0; ii < ajaxText.length - 4; ii+=2){
      var temp = "0x" + ajaxText.substring(ii, ii + 2);
      dataFrame.push(parseInt(temp) * ampScale);
    }
    return dataFrame;
  }
  function changeTimebase(){
    timeScale = parseInt(document.getElementById("timeBase").value);
  }
  function changeVoltsPerDiv(){
    ampScale = ((8.0*scopeHeight)/(10.0*256)) / (parseFloat(document.getElementById("voltsPerDiv").value)/2.0);
    hbctx.clearRect(0, 0, scopeWidth, 40);
    hbctx.strokeText(""+ampScale, hbc.width/2, 30);
  }
  function changeTimerDelay(){
    timerDelay = parseInt(document.getElementById("timerDelay").value);    
  }
</script>
</body>
</html>
)###"; 
