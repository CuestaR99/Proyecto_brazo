let socket = io.connect("192.168.43.91:5000");

let inventario = 0;

let CajasRojas = 0;
let CajasVerdes = 0;
let CajasAzules = 0;

socket.emit("EstadoBrazo", "Estado");

socket.emit("info", "info");

socket.on("datos", (dato) => {
	inventario = dato.inventario;
	CajasRojas = dato.CajasRojas;
	CajasVerdes = dato.CajasVerdes;
	CajasAzules = dato.CajasAzules;
	console.log(
		"La informacion que debe mostrar es la siguiente: \nInventario: %d\nCajas Rojas: %d\nCajas Verdes: %d\nCajas Azules: %d",
		inventario,
		CajasRojas,
		CajasVerdes,
		CajasAzules
	);
	console.log(dato.Estado);
	if (dato.Estado != "Inactivo") {
		document.getElementById("estadoBrazo").innerText = "Brazo Listo";
		document.getElementById("estadoBrazo").style.color = "green";
		document.getElementById("text").disabled = false;
		document.getElementById("boton").disabled = false;
	}
	document.getElementById("Inventario").innerText = "Cantidad: " + inventario;
	document.getElementById("CajasRojas").innerText = "Cantidad: " + CajasRojas;
	document.getElementById("CajasAzules").innerText = "Cantidad: " + CajasAzules;
	document.getElementById("CajasVerdes").innerText = "Cantidad: " + CajasVerdes;
});

socket.on("CajaRoja", (dato) => {
	CajasRojas = dato;
	inventario -=1;
	document.getElementById("Inventario").innerText = "Cantidad: " + inventario;
	document.getElementById("CajasRojas").innerText = "Cantidad: " + CajasRojas;
	document.getElementById("Rojas").style.border = "3px solid #ff0000";
	setTimeout(() => {
		document.getElementById("Rojas").style.border = "none";
	}, 1000);

	console.log("Numero De Cajas Rojas %d", dato);
});

socket.on("CajaAzul", (dato) => {
	CajasAzules = dato;
	inventario--;
	document.getElementById("Inventario").innerText = "Cantidad: " + inventario;
	document.getElementById("CajasAzules").innerText = "Cantidad: " + CajasAzules;
	document.getElementById("Azules").style.border = "3px solid #0000ff";
	setTimeout(() => {
		document.getElementById("Azules").style.border = "none";
	}, 1000);

	console.log("Numero De Cajas Rojas %d", dato);
});

socket.on("CajaVerde", (dato) => {
	CajasVerdes = dato;
    inventario -=1;
    document.getElementById("Inventario").innerText = "Cantidad: " + inventario;
	document.getElementById("CajasVerdes").innerText = "Cantidad: " + CajasVerdes;
	document.getElementById("Verdes").style.border = "3px solid #00ff00";
	setTimeout(() => {
		document.getElementById("Verdes").style.border = "none";
	}, 1000);

	console.log("Numero De Cajas Rojas %d", dato);
});

socket.on("Estado", (valor) => {
	if (valor != "Inactivo") {
		console.log(valor);
		document.getElementById("estadoBrazo").innerText = "Brazo Listo";
		document.getElementById("estadoBrazo").style.color = "green";
		document.getElementById("text").disabled = false;
		document.getElementById("boton").disabled = false;
	}
});

function EnviarInventario() {
	inventario = document.getElementById("text").value;
	if (inventario > 0) {
		socket.emit("Inventario", inventario);
        document.getElementById("Inventario").innerText = "Cantidad: " + inventario;
		document.getElementById("text").value = "";
	}
}
