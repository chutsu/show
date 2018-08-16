class Show {
  constructor() {
    // Websocket
    this.ws = new WebSocket("ws://localhost:8081");
    this.ws.onclose = function() {
      console.log("Websocket closed!");
    }

    // Renderer
    this.width = 680;
    this.height = 480;
    this.aspect_ratio = this.width / this.height;
    this.renderer = new THREE.WebGLRenderer({preserveDrawingBuffer: true});
    this.renderer.setSize(this.width, this.height);
    var container = document.getElementById("show_container");
    container.appendChild(this.renderer.domElement);

    // Stats
    this.stats = new Stats();
    container.appendChild(this.stats.dom);

    // Camera
    this.camera = new THREE.PerspectiveCamera(70, this.aspect_ratio, 1, 10000);
    this.camera.up.set(0, 0, 1);
    this.camera.position.x = -300;
    this.camera.position.y = -300;
    this.camera.position.z = 500;

    // Cube
    var geometry = new THREE.BoxGeometry(10, 10, 10);
    var material = new THREE.MeshBasicMaterial({color: 0x00ff00});
    this.mesh = new THREE.Mesh(geometry, material);

    // Camera controls
    this.controls = new THREE.OrbitControls(this.camera, this.renderer.domElement);
    this.controls.maxPolarAngle = Math.PI / 2;
    this.controls.keys = {
      LEFT: 37, //left arrow
      UP: 38, // up arrow
      RIGHT: 39, // right arrow
      BOTTOM: 40 // down arrow
    };
    this.controls.update();

    // Local frame
    var axes = new THREE.AxesHelper(50);
    axes.material.linewidth = 3;

    // Ground plain grid
    var gridXZ = new THREE.GridHelper(500, 10);
    gridXZ.rotation.x = 90.0 * Math.PI / 180;

    // Scene
    this.scene = new THREE.Scene();
    this.scene.background = new THREE.Color(0xffffff);
    // this.scene.add(this.mesh);
    this.scene.add(axes);
    this.scene.add(gridXZ);

    // Loading manager
    var model;
    var scene = this.scene;
    var loading_manager = new THREE.LoadingManager(function() {
      scene.add(model);
    });

    // Collada model loader
    var loader = new THREE.ColladaLoader(loading_manager);
    loader.load('./models/quadrotor/quadrotor.dae', function(collada) {
      model = collada.scene;
      model.scale.set(50, 50, 50);
      model.position.set(0, 0, 10);
      model.rotation.set(0, 0, 0);
    });

    // Events
    window.addEventListener('resize', function() {that.onWindowResize();}, false);
  }

  onWindowResize() {
    this.camera.aspect = window.innerWidth / window.innerHeight;
    this.camera.updateProjectionMatrix();
    this.renderer.setSize(window.innerWidth, window.innerHeight);
  }

  render() {
    this.mesh.rotation.x += 0.005;
    this.mesh.rotation.y += 0.01;
    this.controls.update();
    this.renderer.render(this.scene, this.camera);

    var that = this;
    requestAnimationFrame(function() {that.render();});
    this.stats.update();
    this.publishScene();
  }

  publishScene() {
    var mime_type = "image/png";
    var imgData = this.renderer.domElement.toDataURL(mime_type);
    if (this.ws.readyState == 1) {
      this.ws.send(imgData);
    }
  }
}

let show = new Show();
show.render();
