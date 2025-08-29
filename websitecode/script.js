let mqttClient;
let user1Active = false;
let user2Active = false;
let user1State = null;
let user2State = null;
let movieCanvasVisible = false;
let youtubePlayer = null;

// MQTT Connection
function connectToMQTT() {
  const mqttStatus = document.getElementById('mqttStatus');
  
  try {
    // Connect to public MQTT broker with secure WebSocket (works for both HTTP and HTTPS)
    mqttClient = mqtt.connect('wss://test.mosquitto.org:8081/mqtt');

    mqttClient.on('connect', function () {
      console.log('Connected to MQTT broker');
      mqttStatus.textContent = 'MQTT Connected ✓';
      mqttStatus.className = 'mqtt-status connected';
      
      // Subscribe to sensor topic (weight sensors for chair activation)
      mqttClient.subscribe('phf-iot-2025/sensors', function (err) {
        if (!err) {
          console.log('Subscribed to phf-iot-2025/sensors');
        }
      });
      
      // Subscribe to button topic (state cycling)
      mqttClient.subscribe('phf-iot-2025/buttons', function (err) {
        if (!err) {
          console.log('Subscribed to phf-iot-2025/buttons');
        }
      });
      
      // Subscribe to video topic (shared video URL)
      mqttClient.subscribe('phf-iot-2025/video', function (err) {
        if (!err) {
          console.log('Subscribed to phf-iot-2025/video');
        }
      });
    });

    mqttClient.on('message', function (topic, message) {
      console.log('Received MQTT:', message.toString(), 'Topic:', topic);
      
      const data = message.toString().split(' ');
      const espId = parseInt(data[0]);
      const value = parseInt(data[1]);
      
      if (topic === 'phf-iot-2025/sensors') {
        // Sensor data: weight sensor activation (1 = sitting, 0 = not sitting)
        if (espId === 1) {
          if (value === 1) {
            activateUser1();
          } else {
            deactivateUser1();
          }
        } else if (espId === 2) {
          if (value === 1) {
            activateUser2();
          } else {
            deactivateUser2();
          }
        }
      } else if (topic === 'phf-iot-2025/buttons') {
        // Button data: state cycling (0=idle, 1=movie, 2=gaming, 3=music)
        const stateNames = ['idle', 'movie', 'gaming', 'music'];
        const stateName = (value >= 0 && value < stateNames.length) ? stateNames[value] : 'idle';
        
        if (espId === 1) {
          setStateUser1(stateName);
        } else if (espId === 2) {
          setStateUser2(stateName);
        }
      } else if (topic === 'phf-iot-2025/video') {
        // Video URL sharing
        const videoUrl = message.toString();
        handleSharedVideoUrl(videoUrl);
      }
    });

    mqttClient.on('error', function (err) {
      console.error('MQTT Error:', err);
      mqttStatus.textContent = 'MQTT Error ✗';
      mqttStatus.className = 'mqtt-status disconnected';
    });

    mqttClient.on('close', function () {
      console.log('MQTT Disconnected');
      mqttStatus.textContent = 'MQTT Disconnected ✗';
      mqttStatus.className = 'mqtt-status disconnected';
    });

  } catch (error) {
    console.error('Failed to connect to MQTT:', error);
    mqttStatus.textContent = 'MQTT Connection Failed ✗';
    mqttStatus.className = 'mqtt-status disconnected';
  }
}

function showElement(id) {
  const element = document.getElementById(id);
  if (element) {
    element.classList.add('visible');
  }
}

function hideElement(id) {
  const element = document.getElementById(id);
  if (element) {
    element.classList.remove('visible');
    element.classList.remove('animated');
  }
}

function activateUser1() {
  user1Active = true;
  showElement('chair_left');
  
  // Set default state to idle if user has no state
  if (!user1State) {
    user1State = 'idle';
  }
  
  // Show state icon
  const elementId = getStateImageId(1, user1State);
  showElement(elementId);
  const element = document.getElementById(elementId);
  if (element) {
    element.classList.add('animated');
  }
  
  updateCampfire();
  checkMovieMode();
  console.log('User 1 activated - state restored:', user1State);
}

function activateUser2() {
  user2Active = true;
  showElement('chair_right');
  
  // Set default state to idle if user has no state
  if (!user2State) {
    user2State = 'idle';
  }
  
  // Show state icon
  const elementId = getStateImageId(2, user2State);
  showElement(elementId);
  const element = document.getElementById(elementId);
  if (element) {
    element.classList.add('animated');
  }
  
  updateCampfire();
  checkMovieMode();
  console.log('User 2 activated - state restored:', user2State);
}

function deactivateUser1() {
  user1Active = false;
  hideElement('chair_left');
  
  // Keep state but hide state icon when user stands up
  if (user1State) {
    hideElement(getStateImageId(1, user1State));
  }
  
  updateCampfire();
  checkMovieMode();
  console.log('User 1 deactivated - state preserved:', user1State);
}

function deactivateUser2() {
  user2Active = false;
  hideElement('chair_right');
  
  // Keep state but hide state icon when user stands up
  if (user2State) {
    hideElement(getStateImageId(2, user2State));
  }
  
  updateCampfire();
  checkMovieMode();
  console.log('User 2 deactivated - state preserved:', user2State);
}

function updateCampfire() {
  const campfireOff = document.getElementById('campfire_off');
  const campfireOn = document.getElementById('campfire_on');
  
  if (user1Active || user2Active) {
    hideElement('campfire_off');
    showElement('campfire_on');
  } else {
    hideElement('campfire_on');
    showElement('campfire_off');
  }
}

function setStateUser1(state) {
  if (!user1Active) {
    console.log('User 1 must be activated first');
    return;
  }

  // Hide previous state
  if (user1State) {
    hideElement(getStateImageId(1, user1State));
  }

  user1State = state;
  const elementId = getStateImageId(1, state);
  showElement(elementId);
  
  // Add animation to state icons
  const element = document.getElementById(elementId);
  if (element) {
    element.classList.add('animated');
  }
  
  // Check if both users are now watching movies
  checkMovieMode();
  
  console.log(`User 1 state set to: ${state}`);
}

function setStateUser2(state) {
  if (!user2Active) {
    console.log('User 2 must be activated first');
    return;
  }

  // Hide previous state
  if (user2State) {
    hideElement(getStateImageId(2, user2State));
  }

  user2State = state;
  const elementId = getStateImageId(2, state);
  showElement(elementId);
  
  // Add animation to state icons
  const element = document.getElementById(elementId);
  if (element) {
    element.classList.add('animated');
  }
  
  // Check if both users are now watching movies
  checkMovieMode();
  
  console.log(`User 2 state set to: ${state}`);
}

function getStateImageId(user, state) {
  const stateMap = {
    'gaming': 'controller',
    'movie': 'popcorn',
    'idle': 'book',
    'music': 'guitar'
  };
  
  const imageType = stateMap[state];
  const side = user === 1 ? 'left' : 'right';
  return `${imageType}_${side}`;
}

function checkMovieMode() {
  const bothUsersInMovieState = user1State === 'movie' && user2State === 'movie';
  const bothUsersActive = user1Active && user2Active;
  const bothUsersDeactivated = !user1Active && !user2Active;
  
  // Show canvas when both users have movie state (regardless of active status)
  if (bothUsersInMovieState && !movieCanvasVisible) {
    showMovieCanvas();
  } 
  // Hide canvas when:
  // 1. ANY user is no longer in movie state (they don't want to watch together), OR
  // 2. Both users are completely deactivated (both stood up)
  else if (movieCanvasVisible && (!bothUsersInMovieState || bothUsersDeactivated)) {
    hideMovieCanvas();
  }
  
  // Handle video play/pause based on ONLY active status when canvas is visible
  if (movieCanvasVisible && bothUsersInMovieState) {
    if (bothUsersActive) {
      resumeVideo();
    } else {
      pauseVideo();
    }
  }
}

function showMovieCanvas() {
  const canvas = document.getElementById('movieCanvas');
  const controls = document.getElementById('movieControls');
  const logo = document.getElementById('logo');
  
  canvas.classList.add('dropped');
  controls.classList.add('visible');
  logo.classList.add('faded');
  movieCanvasVisible = true;
  
  console.log('Movie canvas deployed - both users are watching!');
}

function hideMovieCanvas() {
  const canvas = document.getElementById('movieCanvas');
  const controls = document.getElementById('movieControls');
  const container = document.getElementById('movieVideoContainer');
  const logo = document.getElementById('logo');
  
  canvas.classList.remove('dropped');
  controls.classList.remove('visible');
  logo.classList.remove('faded');
  movieCanvasVisible = false;
  youtubePlayer = null;
  
  // Reset container to placeholder
  container.innerHTML = '<div class="placeholder">🎬 Ready for movie time! Enter a YouTube URL below.</div>';
  
  console.log('Movie canvas hidden - users not both watching movies');
}

function extractVideoId(url) {
  const regExp = /^.*(youtu.be\/|v\/|u\/\w\/|embed\/|watch\?v=|&v=)([^#&?]*).*/;
  const match = url.match(regExp);
  return (match && match[2].length === 11) ? match[2] : null;
}

function loadMovieVideo() {
  const urlInput = document.getElementById('movieUrl');
  const url = urlInput.value.trim();

  if (!url) {
    alert('Please enter a YouTube URL');
    return;
  }

  const videoId = extractVideoId(url);

  if (!videoId) {
    alert('Please enter a valid YouTube URL');
    return;
  }

  // Share the video URL with other users via MQTT
  if (mqttClient && mqttClient.connected) {
    mqttClient.publish('phf-iot-2025/video', url);
    console.log('Video URL shared via MQTT:', url);
  }

  embedMovieVideo(videoId);
}

function embedMovieVideo(videoId) {
  const container = document.getElementById('movieVideoContainer');

  // Create iframe for YouTube embed with enablejsapi for player control
  const iframe = document.createElement('iframe');
  iframe.src = `https://www.youtube.com/embed/${videoId}?autoplay=1&rel=0&modestbranding=1&enablejsapi=1`;
  iframe.allow = 'accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture';
  iframe.allowFullscreen = true;
  iframe.id = 'youtubePlayerIframe';

  // Clear container and add iframe
  container.innerHTML = '';
  container.appendChild(iframe);
  
  // Store reference for pause/resume (simplified approach)
  youtubePlayer = iframe;

  console.log('Movie loaded for both users!');
}

function pauseVideo() {
  if (youtubePlayer) {
    // Send pause command to YouTube iframe
    youtubePlayer.contentWindow.postMessage('{"event":"command","func":"pauseVideo","args":""}', '*');
    console.log('Video paused - user(s) deactivated');
  }
}

function resumeVideo() {
  if (youtubePlayer) {
    // Send play command to YouTube iframe
    youtubePlayer.contentWindow.postMessage('{"event":"command","func":"playVideo","args":""}', '*');
    console.log('Video resumed - both users active');
  }
}

function handleSharedVideoUrl(url) {
  console.log('Received shared video URL:', url);
  
  // Update the input field with the shared URL
  const urlInput = document.getElementById('movieUrl');
  if (urlInput) {
    urlInput.value = url;
  }
  
  // Only auto-load the video if both users are in movie mode
  if (user1State === 'movie' && user2State === 'movie') {
    const videoId = extractVideoId(url);
    if (videoId) {
      embedMovieVideo(videoId);
      console.log('Auto-loaded shared video for both users');
    }
  }
}

// Initialize MQTT connection when page loads
window.addEventListener('load', function() {
  connectToMQTT();
  
  // Allow Enter key to load movie video
  document.getElementById('movieUrl').addEventListener('keypress', function(e) {
    if (e.key === 'Enter') {
      loadMovieVideo();
    }
  });
});