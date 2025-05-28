Trabalho pratico 1 consistem na construção de uma aplicação gráfica utilizando Directx 12 que permita ao usuário criar Curvas de Bézier do tipo cúbica, ou seja, curvas que utilizam 4 pontos de controle, como ilustrado na figura 1.

<img src="Figura1" alt="Figura 1">	<img src="Figura2" alt="Figura 2">
Figura 1			Figura 2

A aplicação deve se comportar da seguinte forma:
• A posição dos	pontos P1, P2, P3 e P4 deve ser definida a partir da posição do	mouse, com o pressionamento do botão esquerdo do mouse.
• Depois que uma curva estiver definida, pressionamentos subsequentes do botão do mouse	devem gerar novas curvas conectadas a curva anterior, formando curvas contínuas, como mostrado na figura 2.	
• O pressionamento da tecla DEL	deve apagar a curva e limpar a tela, permitindo o desenho de uma nova curva.
• O pressionamento da tecla S (Save) deve salvar a curva e todos os parâmetros necessários para	poder restaurar	a curva.	
• O pressionamento da tecla L (Load) deve restaurar a última curva armazenada e permitir que ela continue sendo	ampliada a partir do mesmo ponto do último salvamento. Qualquer curva existente na	tela no	momento	do carregamento	deve ser descartada.
• O programa deve permitir a criação de	pelo menos 20 curvas de	Bézier conectadas.