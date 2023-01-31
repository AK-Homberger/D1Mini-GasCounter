const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta HTTP-EQUIV="Content-Type" CONTENT="text/html; charset=UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
h1 {
  font-size: 1.5em;
  text-align: center; 
  vertical-align: middle; 
  margin:0 auto;
}
p {
  font-size: 1.5em;
  text-align: center; 
  vertical-align: middle; 
  margin:0 auto;
}
table {
  font-size: 1.5em;
  text-align: left; 
  vertical-align: middle; 
  margin:0 auto;
}
.button {
  font-size: 22px;;
  text-align: center; 
}
.slidecontainer {
  width: 100%;
}
.slider {
  -webkit-appearance: none;
  width: 80%;
  height: 22px;
  background: #d3d3d3;
  outline: none;
  opacity: 0.7;
  -webkit-transition: .2s;
  transition: opacity .2s;
}
.slider:hover {
  opacity: 1;
}
.slider::-webkit-slider-thumb {
  -webkit-appearance: none;
  appearance: none;
  width: 18px;
  height: 18px;
  background: #4CAF50;
  cursor: pointer;
}
.slider::-moz-range-thumb {
  width: 18px;
  height: 18px;
  background: #4CAF50;
  cursor: pointer;
}
</style>
<title>Gaszähler</title>
<hr>
<h1>Gaszähler</h1>
<hr>
</head>
<body style="font-family: verdana,sans-serif" BGCOLOR="#819FF7">
  <table>
    <tr><td style="text-align:right;">Zeit:</td><td style="color:white;"><span id='time'></span> </td></tr>
    <tr><td style="text-align:right;">Stand:</td><td style="color:white;"><span id='stand'></span> m³ </td></tr>
    <tr><td style="text-align:right;">Heute:</td><td style="color:white;"><span id='heute'></span> m³ </td></tr>
    <tr><td style="text-align:right;">Gestern:</td><td style="color:white;"><span id='gestern'></span> m³ </td></tr>
    <tr><td style="text-align:right;">Aktuell:</td><td style="color:white;"><span id='m3h_akt'></span> m³/h </td></tr>
    <tr><td style="text-align:right;">Stunde:</td><td style="color:white;"><span id='m3h'></span> m³/h </td></tr>
    <tr><td style="text-align:right;">Tag:</td><td style="color:white;"><span id='tag'></span> m³/d </td></tr>
  </table>
  <br>
  <hr>
  <p>
  <input type="button" class="button" id='set' value=' Stellen ' onclick="button_clickedSend()"> 
  <input type="button" class="button" id='reset' value=' Reset ' onclick="button_reset()"> 
  </p>
  
  <script>
 
    requestData(); // get intial data straight away 
           
    // request data updates every 500 milliseconds
    setInterval(requestData, 500);
    
    function requestData() {
      var xhr = new XMLHttpRequest();
      
      xhr.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
          if (this.responseText) { // if the returned data is not null, update the values
            var data = JSON.parse(this.responseText);
            document.getElementById("time").innerText = data.time;                        
            document.getElementById("stand").innerText = data.stand;
            document.getElementById("m3h_akt").innerText = data.m3h_akt;
            document.getElementById("m3h").innerText = data.m3h;
            document.getElementById("tag").innerText = data.tag;
            document.getElementById("heute").innerText = data.heute;
            document.getElementById("gestern").innerText = data.gestern;
          } 
        } 
      }
      xhr.open('GET', 'get_data', true);
      xhr.send();
    }

    function button_clickedSend() { 
      var Text = "";
      var Event = "";
      var URL = "";
      
      Text = prompt("Neuer Zählerstand:", '');
      
      if (Text == "" || Text == null) return;        
    
      Event = Text;
      
      URL = "set_data";        
      var xhr = new XMLHttpRequest();  
      xhr.open('GET', URL + '?count=' + Event, true);
      xhr.send();      
    }


    function button_reset() {       
     
      var xhr = new XMLHttpRequest();  
      xhr.open('GET','reset', true);
      xhr.send();      
    }    
     
  </script>
</body>
</html>
)=====";
