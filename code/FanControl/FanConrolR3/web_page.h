const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html lang="de">
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta charset="UTF-8">
<title>IoT Klima Controller</title>  
  <link rel="preconnect" href="https://fonts.googleapis.com">
  <link href="https://fonts.googleapis.com/css2?family=Share+Tech+Mono&family=Titillium+Web:wght@400;600;700&display=swap" rel="stylesheet">
  
  <style>
    /* ================= CSS VARIABLEN (INDUSTRIAL TECH THEME) ================= */
    :root {
      --bg-dark: #0d1117;
      --panel-bg: #161b22;
      --border-color: #30363d;
      --text-main: #c9d1d9;
      --text-highlight: #58a6ff;
      --accent: #2f81f7;
      --success: #2ea043;
      --warning: #d29922;
      --danger: #f85149;
      --gauge-bg: #21262d;
    }

    /* ================= GLOBALES LAYOUT ================= */
    body {
      margin: 0;
      font-family: 'Titillium Web', sans-serif;
      background-color: var(--bg-dark);
      background-image: 
        linear-gradient(rgba(255, 255, 255, 0.02) 1px, transparent 1px),
        linear-gradient(90deg, rgba(255, 255, 255, 0.02) 1px, transparent 1px);
      background-size: 30px 30px; /* Raster-Hintergrund für Tech-Look */
      color: var(--text-main);
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    /* ================= HEADER STYLING ================= */
    .header {
      width: 100%;
      padding: 15px 30px;
      background: rgba(22, 27, 34, 0.85);
      border-bottom: 2px solid var(--accent);
      display: flex;
      justify-content: space-between;
      align-items: center;
      box-sizing: border-box;
      backdrop-filter: blur(5px);
    }
    
    h1 {
      margin: 0;
      font-size: 1.4rem;
      font-weight: 700;
      letter-spacing: 2px;
      color: #ffffff;
      text-transform: uppercase;
    }

    .live-indicator {
      display: flex;
      align-items: center;
      gap: 8px;
      font-family: 'Share Tech Mono', monospace;
      color: var(--success);
      font-size: 0.9rem;
    }

    .blink-dot {
      width: 10px; height: 10px;
      background-color: var(--success);
      border-radius: 50%;
      animation: blink 1.5s infinite;
      box-shadow: 0 0 8px var(--success);
    }
    
    @keyframes blink { 0%, 100% { opacity: 1; } 50% { opacity: 0.3; } }

    /* ================= DASHBOARD GRID ================= */
    .dashboard {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
      gap: 25px;
      padding: 30px;
      width: 100%;
      max-width: 1200px;
      box-sizing: border-box;
    }

    /* ================= KARTEN (PANELS) ================= */
    .panel {
      background: var(--panel-bg);
      border: 1px solid var(--border-color);
      border-radius: 6px;
      padding: 20px;
      position: relative;
      box-shadow: 0 8px 24px rgba(0,0,0,0.4);
      display: flex;
      flex-direction: column;
      align-items: center;
    }

    /* Dekorative Ecken für den Industrie-Look */
    .panel::before, .panel::after {
      content: ''; position: absolute; width: 15px; height: 15px;
    }
    .panel::before { top: -1px; left: -1px; border-top: 2px solid var(--text-highlight); border-left: 2px solid var(--text-highlight); }
    .panel::after { bottom: -1px; right: -1px; border-bottom: 2px solid var(--text-highlight); border-right: 2px solid var(--text-highlight); }

    .panel-title {
      font-size: 0.85rem;
      color: #8b949e;
      text-transform: uppercase;
      letter-spacing: 2px;
      width: 100%;
      border-bottom: 1px solid var(--border-color);
      padding-bottom: 10px;
      margin-bottom: 20px;
      text-align: left;
    }

    /* ================= SVG GAUGES (FORTSCHRITTSKREISE) ================= */
    .gauge-container {
      position: relative;
      width: 160px;
      height: 160px;
      display: flex;
      justify-content: center;
      align-items: center;
    }

    .circular-chart { display: block; margin: 0 auto; max-width: 80%; max-height: 250px; }
    
    .circle-bg {
      fill: none;
      stroke: var(--gauge-bg);
      stroke-width: 2.5;
    }

    .circle {
      fill: none;
      stroke-width: 2.5;
      stroke-linecap: round;
      transition: stroke-dasharray 1s ease-out, stroke 0.5s ease;
    }

    .gauge-data {
      position: absolute;
      text-align: center;
      font-family: 'Share Tech Mono', monospace;
    }

    .value-display { font-size: 2.5rem; color: #ffffff; text-shadow: 0 0 10px rgba(255,255,255,0.2); }
    .unit { font-size: 1rem; color: var(--text-highlight); }
    .avg-label { font-size: 0.75rem; color: #8b949e; margin-top: 5px; }

    /* ================= LÜFTER / SYSTEMSTATUS ================= */
    .system-status-container {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      flex-grow: 1;
    }

    .fan-wrapper { width: 100px; height: 100px; margin: 15px 0; }
    
    .fan-svg {
      width: 100%; height: 100%;
      fill: #484f58;
      transition: fill 0.3s ease;
    }

    .spinning {
      animation: spin 0.3s linear infinite;
      fill: var(--danger) !important;
      filter: drop-shadow(0 0 12px rgba(248, 81, 73, 0.6));
    }

    @keyframes spin { 0% { transform: rotate(0deg); } 100% { transform: rotate(360deg); } }

    .status-badge {
      padding: 8px 24px;
      border-radius: 4px;
      font-size: 0.9rem;
      font-weight: 700;
      letter-spacing: 1px;
      background: rgba(46, 160, 67, 0.1);
      border: 1px solid var(--success);
      color: var(--success);
      text-transform: uppercase;
      box-shadow: 0 0 10px rgba(46, 160, 67, 0.2);
    }

    /* ================= KONTROLLFELD ================= */
    .control-group {
      display: flex;
      flex-direction: column;
      gap: 20px;
      width: 100%;
      align-items: center;
      justify-content: center;
      flex-grow: 1;
    }

    .input-wrapper {
      position: relative;
      display: flex;
      align-items: center;
    }

    .input-wrapper::after {
      content: '°C';
      position: absolute;
      right: 15px;
      font-family: 'Share Tech Mono', monospace;
      color: #8b949e;
      font-size: 1.2rem;
    }

    input[type=number] {
      background: var(--bg-dark);
      border: 1px solid var(--border-color);
      color: #ffffff;
      padding: 15px 40px 15px 20px;
      border-radius: 4px;
      font-size: 1.8rem;
      font-family: 'Share Tech Mono', monospace;
      text-align: center;
      width: 110px;
      outline: none;
      transition: border-color 0.3s, box-shadow 0.3s;
    }
    
    input[type=number]:focus { border-color: var(--text-highlight); box-shadow: 0 0 10px rgba(88, 166, 255, 0.3); }

    button.btn-save {
      background: transparent;
      color: var(--text-highlight);
      border: 1px solid var(--text-highlight);
      padding: 12px 0;
      border-radius: 4px;
      font-size: 0.9rem;
      font-weight: 700;
      letter-spacing: 2px;
      text-transform: uppercase;
      cursor: pointer;
      transition: all 0.2s;
      width: 80%;
      font-family: 'Titillium Web', sans-serif;
    }

    button.btn-save:hover { background: rgba(88, 166, 255, 0.1); box-shadow: 0 0 15px rgba(88, 166, 255, 0.2); }
    button.btn-save:active { transform: scale(0.96); }

  </style>
</head>
<body>

  <div class="header">
    <h1>Systemsteuerung</h1>
    <div class="live-indicator">
      <div id="connection-dot" class="blink-dot"></div>
      <span id="connection-text">SYS ONLINE</span>
    </div>
  </div>

  <div class="dashboard">
    
    <!-- PANEL 1: TEMPERATUR (MIT SVG GAUGE) -->
    <div class="panel">
      <div class="panel-title">Temperatursensor</div>
      <div class="gauge-container">
        <svg viewBox="0 0 36 36" class="circular-chart">
          <path class="circle-bg" d="M18 2.0845 a 15.9155 15.9155 0 0 1 0 31.831 a 15.9155 15.9155 0 0 1 0 -31.831" />
          <path id="temp-circle" class="circle" stroke="#58a6ff" stroke-dasharray="0, 100" d="M18 2.0845 a 15.9155 15.9155 0 0 1 0 31.831 a 15.9155 15.9155 0 0 1 0 -31.831" />
        </svg>
        <div class="gauge-data">
          <div><span id="temp-val" class="value-display">--</span><span class="unit">°C</span></div>
          <div class="avg-label">Ø <span id="avg">--</span>°C</div>
        </div>
      </div>
    </div>

    <!-- PANEL 2: LUFTFEUCHTIGKEIT (MIT SVG GAUGE) -->
    <div class="panel">
      <div class="panel-title">Luftfeuchtigkeit</div>
      <div class="gauge-container">
        <svg viewBox="0 0 36 36" class="circular-chart">
          <path class="circle-bg" d="M18 2.0845 a 15.9155 15.9155 0 0 1 0 31.831 a 15.9155 15.9155 0 0 1 0 -31.831" />
          <path id="hum-circle" class="circle" stroke="#2ea043" stroke-dasharray="0, 100" d="M18 2.0845 a 15.9155 15.9155 0 0 1 0 31.831 a 15.9155 15.9155 0 0 1 0 -31.831" />
        </svg>
        <div class="gauge-data">
          <div><span id="hum-val" class="value-display">--</span><span class="unit">%</span></div>
          <div class="avg-label">RELATIV</div>
        </div>
      </div>
    </div>

    <!-- PANEL 3: LÜFTER STATUS -->
    <div class="panel">
      <div class="panel-title">Aktive Kühlung</div>
      <div class="system-status-container">
        <div class="fan-wrapper">
          <svg class="fan-svg" id="fan-icon" viewBox="0 0 24 24">
             <path d="M12,12c0-3,2.5-5.5,5.5-5.5S23,9,23,12H12z" />
             <path d="M12,12c3,0,5.5,2.5,5.5,5.5S15,23,12,23V12z" />
             <path d="M12,12c0,3-2.5,5.5-5.5,5.5S1,15,1,12H12z" />
             <path d="M12,12c-3,0-5.5-2.5-5.5-5.5S9,1,12,1V12z" />
             <circle cx="12" cy="12" r="3" fill="#0d1117" />
             <circle cx="12" cy="12" r="1.5" fill="currentColor" opacity="0.5"/>
          </svg>
        </div>
        <div id="status-text" class="status-badge">SYSTEM STABIL</div>
      </div>
    </div>

    <!-- PANEL 4: PARAMETER -->
    <div class="panel">
      <div class="panel-title">Alarm-Parameter</div>
      <div class="control-group">
        <div class="input-wrapper">
          <input type="number" id="threshold-input" step="0.1" value="%LIMIT%">
        </div>
        <button class="btn-save" onclick="saveConfig()">PARAMETER ÜBERSCHREIBEN</button>
      </div>
    </div>

  </div>

<script>
    // ================= JAVASCRIPT LOGIK =================

    setInterval(updateData, 1000);

    function updateData() {
      fetch('/status?nocache=' + Date.now())
        .then(response => response.json())
        .then(data => {
          
          document.getElementById('temp-val').innerText = data.temp;
          document.getElementById('hum-val').innerText = data.hum;
          document.getElementById('avg').innerText = data.avg;
          
          // Berechne Kreis-Füllung (Max Temp angenommen bei 50°C für die Skala)
          const tempPercent = (data.temp / 50) * 100;
          document.getElementById('temp-circle').setAttribute('stroke-dasharray', `${tempPercent}, 100`);
          
          // Feuchtigkeit ist bereits 0-100%
          document.getElementById('hum-circle').setAttribute('stroke-dasharray', `${data.hum}, 100`);

          const statusTxt = document.getElementById('status-text');
          const fan = document.getElementById('fan-icon');
          const tempCircle = document.getElementById('temp-circle');
          
          fan.classList.remove('spinning');

          // --- LOGIK & FARB-UPDATES ---
          if(data.status == 2) { 
            // KRITISCH
            tempCircle.style.stroke = "var(--danger)";
            
            statusTxt.innerText = "KÜHLUNG AKTIV";
            statusTxt.style.color = "var(--danger)";
            statusTxt.style.borderColor = "var(--danger)";
            statusTxt.style.background = "rgba(248, 81, 73, 0.1)";
            statusTxt.style.boxShadow = "0 0 15px rgba(248, 81, 73, 0.3)";
            
            fan.classList.add('spinning');
            
          } else if(data.status == 1) { 
            // WARNUNG
            tempCircle.style.stroke = "var(--warning)";
            
            statusTxt.innerText = "WARNUNG: TEMP HOCH";
            statusTxt.style.color = "var(--warning)";
            statusTxt.style.borderColor = "var(--warning)";
            statusTxt.style.background = "rgba(210, 153, 34, 0.1)";
            statusTxt.style.boxShadow = "none";

          } else { 
            // NORMAL
            tempCircle.style.stroke = "var(--text-highlight)";
            
            statusTxt.innerText = "SYSTEM STABIL";
            statusTxt.style.color = "var(--success)";
            statusTxt.style.borderColor = "var(--success)";
            statusTxt.style.background = "rgba(46, 160, 67, 0.1)";
            statusTxt.style.boxShadow = "0 0 10px rgba(46, 160, 67, 0.2)";
          }
        })
        .catch(error => {
          console.error("Datenabruf fehlgeschlagen:", error);
          document.getElementById('connection-dot').style.backgroundColor = "var(--danger)";
          document.getElementById('connection-text').innerText = "OFFLINE";
          document.getElementById('connection-text').style.color = "var(--danger)";
        });
    }

    function saveConfig() {
      const btn = document.querySelector('.btn-save');
      const val = document.getElementById('threshold-input').value;
      const originalText = btn.innerText;

      btn.innerText = "DATEN TRANSFERIEREN...";
      
      fetch('/set?threshold=' + val)
        .then(res => {
          if(res.ok) {
            btn.innerText = "ERFOLGREICH";
            btn.style.color = "var(--success)";
            btn.style.borderColor = "var(--success)";
            
            setTimeout(() => {
              btn.innerText = originalText;
              btn.style.color = "var(--text-highlight)";
              btn.style.borderColor = "var(--text-highlight)";
            }, 2000);
          }
        })
        .catch(err => {
          alert("Verbindungsfehler!");
          btn.innerText = originalText;
        });
    }
</script>
</body>
</html>
)rawliteral";
