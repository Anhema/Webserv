const products = [
	{
		nombre: 'Alubias blancas',
		descripcion: 'Alubias blancas de tamaño medio, cultivadas en La Rioja.',
		precio: 2.5,
		imagen: 'alubias-blancas.jpg'
	},
	{
		nombre: 'Alubias negras',
		descripcion: 'Alubias negras de tamaño pequeño, cultivadas en La Rioja.',
		precio: 3,
		imagen: 'alubias-negras.jpg'
	},
	{
		nombre: 'Alubias pintas',
		descripcion: 'Alubias pintas de tamaño grande, cultivadas en La Rioja.',
		precio: 2,
		imagen: 'alubias-pintas.png'
	}
];

const productTable = document.getElementById('product-table');
for (let i = 0; i < products.length; i++) {
	const product = products[i];
	const row = document.createElement('tr');
	row.innerHTML = `
		<td><img src="${product.imagen}" alt="${product.nombre}"></td>
		<td>${product.nombre}</td>
		<td>${product.descripcion}</td>
		<td>${product.precio.toFixed(2)} €/kg</td>
	`;
	productTable.appendChild(row);
}