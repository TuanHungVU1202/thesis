//var classes = ['amy', 'raj', 'stuart', 'va', 'hung', 'pa']
var classes = []

function getImageUri(imageName) {
  return `images/${imageName}`
}

function getFaceImageUri(className, idx) {
  return `images/${className}/${className}${idx}.png`
}

async function fetchImage(uri) {
  return (await fetch(uri)).blob()
}

async function requestExternalImage(imageUrl) {
  const res = await fetch('fetch_external_image', {
    method: 'post',
    headers: {
      'content-type': 'application/json'
    },
    body: JSON.stringify({ imageUrl })
  })
  if (!(res.status < 400)) {
    console.error(res.status + ' : ' + await res.text())
    throw new Error('failed to fetch image from url: ' + imageUrl)
  }

  let blob
  try {
    blob = await res.blob()
    return await faceapi.bufferToImage(blob)
  } catch (e) {
    console.error('received blob:', blob)
    console.error('error:', e)
    throw new Error('failed to load image from url: ' + imageUrl)
  }
}

// fetch first image of each class and compute their descriptors
async function initTrainDescriptorsByClass(net, numImagesForTraining = 5) {
  const maxAvailableImagesPerClass = 5
  numImagesForTraining = Math.min(numImagesForTraining, maxAvailableImagesPerClass)
  return Promise.all(classes.map(
    async className => {
      const descriptors = []
      for (let i = 1; i < (numImagesForTraining + 1); i++) {
        const img = await faceapi.bufferToImage(
          await fetchImage(getFaceImageUri(className, i))
        )
        descriptors.push(await net.computeFaceDescriptor(img))
      }
      return {
        descriptors,
        className
      }
    }
  ))
}

function getBestMatch(descriptorsByClass, queryDescriptor) {
  function computeMeanDistance(descriptorsOfClass) {
    return faceapi.round(
      descriptorsOfClass
        .map(d => faceapi.euclideanDistance(d, queryDescriptor))
        .reduce((d1, d2) => d1 + d2, 0)
          / (descriptorsOfClass.length || 1)
      )
  }
  return descriptorsByClass
    .map(
      ({ descriptors, className }) => ({
        distance: computeMeanDistance(descriptors),
        className
      })
    )
    .reduce((best, curr) => best.distance < curr.distance ? best : curr)
}

