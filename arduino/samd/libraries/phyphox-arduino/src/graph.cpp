#include "phyphoxBleExperiment.h"

void PhyphoxBleExperiment::Graph::setUnitX(const char *ux){
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkLength(ux, 5, "setUnitX") : ERROR;
	memset(&UNITX[0], 0, sizeof(UNITX));
	strcat(UNITX, " unitX=\"");
	strcat(UNITX, ux);
	strcat(UNITX, "\"");
}

void PhyphoxBleExperiment::Graph::setUnitY(const char *uy){
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkLength(uy, 5, "setUnitY") : ERROR;
	memset(&UNITY[0], 0, sizeof(UNITY));
	strcat(UNITY, " unitY=\"");
	strcat(UNITY, uy);
	strcat(UNITY, "\"");
}

void PhyphoxBleExperiment::Graph::setLabelX(const char *lx){
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkLength(lx, 20, "setLabelX") : ERROR;
	memset(&LABELX[0], 0, sizeof(LABELX));
	strcat(LABELX, " labelX=\"");
	strcat(LABELX, lx);
	strcat(LABELX, "\"");
}

void PhyphoxBleExperiment::Graph::setLabelY(const char *ly){
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkLength(ly, 20, "setLabelX") : ERROR;
	memset(&LABELY[0], 0, sizeof(LABELY));
	strcat(LABELY, " labelY=\"");
	strcat(LABELY, ly);
	strcat(LABELY, "\"");
}

void PhyphoxBleExperiment::Graph::setColor(const char *c){
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkHex(c, "setColor") : ERROR;
	memset(&COLOR[0], 0, sizeof(COLOR));
	strcat(COLOR, " color=\"");
	strcat(COLOR, c);
	strcat(COLOR, "\"");
}

void PhyphoxBleExperiment::Graph::setXPrecision(int px){
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkUpper(px, 9999, "setXPrecision") : ERROR;
	sprintf(XPRECISION, " xPrecision=\"%i\"", px);
}

void PhyphoxBleExperiment::Graph::setYPrecision(int py){
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkUpper(py, 9999, "setYPrecision") : ERROR;
	sprintf(YPRECISION, " yPrecision=\"%i\"", py);
}

void PhyphoxBleExperiment::Graph::setChannel(int x, int y)
{
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkUpper(x, 5, "setChannel") : ERROR;
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkUpper(y, 5, "setChannel") : ERROR;
	sprintf(INPUTX, "CH%i", x);
	sprintf(INPUTY, "CH%i", y);
}

void PhyphoxBleExperiment::Graph::setStyle(const char *s){
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkStyle(s, "setStyle") : ERROR;
	memset(&STYLE[0], 0, sizeof(STYLE));
	strcat(STYLE, " style=\"");
	strcat(STYLE, s);
	strcat(STYLE, "\"");
}

void PhyphoxBleExperiment::Graph::setXMLAttribute(const char *xml){
	ERROR = (strcmp(ERROR.MESSAGE, "")==0) ? err_checkLength(xml, 98, "setXMLAttribute") : ERROR;
	memset(&XMLAttribute[0], 0, sizeof(XMLAttribute));
	strcat(XMLAttribute, " ");
	strcat(XMLAttribute, xml);
}

void PhyphoxBleExperiment::Graph::getBytes(char *buffArray)
{
	strcat(buffArray,"\t\t<graph");
	strcat(buffArray,LABEL);
	strcat(buffArray,LABELX);
	strcat(buffArray,LABELY);
	//strcat(buffArray," labelZ=\"\"");
	strcat(buffArray,UNITX);
	strcat(buffArray,UNITY);
	strcat(buffArray,XPRECISION);
	strcat(buffArray,YPRECISION);
	strcat(buffArray,STYLE);
	strcat(buffArray,COLOR);
	strcat(buffArray,XMLAttribute);
	strcat(buffArray,">\n");

	strcat(buffArray, "\t\t\t<input axis=\"x\">");
	strcat(buffArray, INPUTX);
	strcat(buffArray, "</input>\n\t\t\t<input axis=\"y\">");
	strcat(buffArray, INPUTY);
	strcat(buffArray, "</input>\n\t\t</graph>\n");	
}
