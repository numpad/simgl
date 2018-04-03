
Console console;
Camera scam(0.0f, 0.0f, 0.0f);

void on_load() {
	console.log("Debug Console - AngelScript");
	
	//RegisterCamera(scam);

}

void on_render() {
	console.render();
}

