/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

'use strict';

var errorElement = document.querySelector('#errorMsg');

//var video1 = document.querySelector('video#video1');

var pipes = [];
//var localStream;
//var remoteStream;

// Put variables in global scope to make them available to the browser console.
/*var constraints = window.constraints = {
  audio: false,
  video: true
};
*/

function openStream(){
    const config = { audio: true, video: true };
    return navigator.mediaDevices.getUserMedia(config);
};

function playStream(idVidTag, stream) {
  trace('Received local stream');
  const video1 = document.getElementById(idVidTag);
  video1.srcObject = stream;
  video1.play();
  //window.localStream = stream;
  //callButton.disabled = false;
}


function handleError(error) {
  if (error.name === 'ConstraintNotSatisfiedError') {
    errorMsg('The resolution ' + constraints.video.width.exact + 'x' +
        constraints.video.width.exact + ' px is not supported by your device.');
  } else if (error.name === 'PermissionDeniedError') {
    errorMsg('Permissions have not been granted to use your camera and ' +
      'microphone, you need to allow the page access to your devices in ' +
      'order for the demo to work.');
  }
  errorMsg('getUserMedia error: ' + error.name, error);
}

function errorMsg(msg, error) {
  errorElement.innerHTML += '<p>' + msg + '</p>';
  if (typeof error !== 'undefined') {
    console.error(error);
  }
}

openStream()
    .then(stream => playStream('localVideo',stream)).catch(handleError);

/*navigator.mediaDevices.getUserMedia(constraints).
  then(playStream).catch(handleError);
  */
