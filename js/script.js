class Show {
  constructor() {
		// // Websocket
		// var ws = new WebSocket("ws://localhost:8081");
		// ws.onclose = function() {
		// 	console.log("Websocket closed!");
		// }

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

		// Helpers
		var axes = new THREE.AxesHelper(50);
		console.log(axes);
		axes.material.linewidth = 3;
		var gridXZ = new THREE.GridHelper(500, 10);
    gridXZ.rotation.x = 90.0 * Math.PI / 180;

    // Scene
    this.scene = new THREE.Scene();
    this.scene.add(this.mesh);
		this.scene.add(axes);
		this.scene.add(gridXZ);

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
  }

	publishScene() {
		var mime_type = "image/png";
		var imgData = renderer.domElement.toDataURL(mime_type);
		if (ws.readyState == 1) {
			ws.send(imgData);
		}
	}
}

let show = new Show();
show.render();
