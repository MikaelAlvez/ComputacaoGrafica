Trabalho pratico 1:
Consistem na construção de uma aplicação gráfica utilizando Directx 12 que permita ao usuário criar Curvas de Bézier do tipo cúbica, ou seja, curvas que utilizam 4 pontos de controle, como ilustrado na figura 1.

![Captura de tela 2025-05-28 194646](https://github.com/user-attachments/assets/00fea443-bbb7-479c-8567-4e7a84913a56)

A aplicação deve se comportar da seguinte forma:
• A posição dos	pontos P1, P2, P3 e P4 deve ser definida a partir da posição do	mouse, com o pressionamento do botão esquerdo do mouse.
• Depois que uma curva estiver definida, pressionamentos subsequentes do botão do mouse	devem gerar novas curvas conectadas a curva anterior, formando curvas contínuas, como mostrado na figura 2.	
• O pressionamento da tecla DEL	deve apagar a curva e limpar a tela, permitindo o desenho de uma nova curva.
• O pressionamento da tecla S (Save) deve salvar a curva e todos os parâmetros necessários para	poder restaurar	a curva.	
• O pressionamento da tecla L (Load) deve restaurar a última curva armazenada e permitir que ela continue sendo	ampliada a partir do mesmo ponto do último salvamento. Qualquer curva existente na	tela no	momento	do carregamento	deve ser descartada.
• O programa deve permitir a criação de	pelo menos 20 curvas de	Bézier conectadas.

Trabalho pratico 2:
A aplicação gráfica permite ao usuário visualizar e editar uma cena 3D formada a partir das geometrias Box, Cylinder, Sphere, GeoSphere, Grid, Quad e também geometrias carregadas a partir de arquivos OBJ.

| Tecla | Figura       |
| ----- | ------------ |
| **B** | Box          |
| **C** | Cylinder     |
| **S** | Sphere       |
| **G** | GeoSphere    |
| **P** | Plane (Grid) |

| Tecla     | Ação                 |
| --------- | -------------------- |
| **Q**     | Quad                 |
| **1 a 5** | Arquivos             |
| **TAB**   | Seleciona            |
| **DEL**   | Remove               |
| **V**     | Modo de Visualização |

Combinação de teclas do teclado e/ou mouse para fazer a translação, escala e rotação da figura selecionada:
-Aumentar escala: CTRL + E + Seta cima
-Diminuir escala: CTRL + E + Seta baixo

-Rotacionar no eixo X: CTRL + X + R
-Rotacionar no eixo Y: CTRL + Y + R
-Rotacionar no eixo Z: CTRL + Z + R

-Transacionar no eixo X para Direira: CTRL + X + Seta direita
-Transacionar no eixo X para Esquerda: CTRL + X + Seta esquerda

-Transacionar no eixo Y para Cima: CTRL + Y + Seta cima
-Transacionar no eixo Y para Direira: CTRL + Y + Seta baixo

-Transacionar no eixo Z para Tras: CTRL + Z + Seta direita
-Transacionar no eixo Z para frente: CTRL + Z + Seta esquerda

A tecla V modifica o modo de visualização, apresentando a cena em 4 vistas diferentes: Front, Top, Right e Perspective. Com exceção da perspectiva, as visualizações devem usar uma projeção ortográfica na direção correspondente.

Clicar e arrastar o botão esquerdo do mouse gira a câmera usando coordenadas esféricas. Clicar e arrastar o botão direito deve aproxima e afasta a câmera da cena. Apenas a câmera da visualização perspectiva é afetada por essas interações. As visualizações ortográficas não são alteradas.

As teclas numéricas carrega modelos 3D a partir de arquivos: 1-Thorus, 2-Monkey, 3-House, 4-Ball e 5-Capsule.
