module prob2(n11, n12, n13, n1, n2, n3); 
	output n11, n12, n13;   
	input n1, n2, n3;
	//internal wires
	wire n4, n5, n6, n7, n8, n9, n10;
	INVX1 g1(.ZN(n4), .I(n1));   
	INVX1 g2(.ZN(n5), .I(n2));   
	NANDX1 g3(.ZN(n6), .A1(n4), .A2(n5));   
	INVX1 g4(.ZN(n7), .I(n5));   
	NOR2X1 g5(.ZN(n8), .A1(n4), .A2(n3));   
	INVX1 g6(.ZN(n9), .I(n6));   
	NOR2X1 g7(.ZN(n10), .A1(n6), .A2(n7));   
	NANDX1 g8(.ZN(n11), .A1(n7), .A2(n8));   
	NOR2X1 g9(.ZN(n12), .A1(n9), .A2(n10));   
	NOR2X1 g10(.ZN(n13), .A1(n10), .A2(n8));   
endmodule



