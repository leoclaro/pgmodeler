#include "textboxview.h"

TextboxView::TextboxView(Textbox *cxtexto, const QBrush &brush, const QPen &pen) : BaseObjectView(cxtexto)
{
 connect(cxtexto, SIGNAL(s_objectModified(void)), this, SLOT(configureObject(void)));

 //Aloca os objetos que definem a representação gráfica de caixa de texto
 caixa=new QGraphicsPolygonItem;
 texto=new QGraphicsSimpleTextItem;

 //Caso o brush ou pen não estejam especificados usa a cor padrão da caixa de texto
 if(brush.style()==Qt::NoBrush || pen.style()==Qt::NoPen)
 {
  caixa->setBrush(this->getFillStyle(BaseObject::getSchemaName(OBJ_TEXTBOX)));
  caixa->setPen(this->pen=this->getBorderStyle(BaseObject::getSchemaName(OBJ_TEXTBOX)));
 }
 else
 {
  //Configura as cores da caixa de texto
  caixa->setBrush(brush);
  caixa->setPen(pen);
 }

 //Agrupa os objetos alocados e efetua a configuração gráfica da caixa de texto
 this->addToGroup(texto);
 this->addToGroup(caixa);
 this->configureObject();
}

TextboxView::~TextboxView(void)
{
 disconnect(this, SLOT(configureObject(void)));

 this->removeFromGroup(caixa);
 this->removeFromGroup(texto);
 delete(caixa);
 delete(texto);
}

void TextboxView::configureObject(void)
{
 Textbox *cxtexto=dynamic_cast<Textbox *>(this->getSourceObject());
 QTextCharFormat fmt=font_config[ParsersAttributes::GLOBAL];
 QFont fonte;
 QPolygonF poligono;

 //Cria o polígono que define a caixa de texto em si
 poligono.append(QPointF(0.0f,0.0f));
 poligono.append(QPointF(1.0f,0.0f));
 poligono.append(QPointF(1.0f,1.0f));
 poligono.append(QPointF(0.0f,1.0f));

 //A caixa deve ficar abaixo dos demais objetos por isso um Z=0
 caixa->setZValue(0);

 /* Configura o estilo da fonte do texto conforme os parâmetros definidos
    na caixa de texto que deu origem ao objeto gráfico */
 fonte=fmt.font();
 fonte.setItalic(cxtexto->getTextAttribute(Textbox::ITALIC_TXT));
 fonte.setBold(cxtexto->getTextAttribute(Textbox::BOLD_TXT));
 fonte.setUnderline(cxtexto->getTextAttribute(Textbox::UNDERLINE_TXT));

 //Configura o texto e o estilo de fonte da caixa
 texto->setText(QString::fromUtf8(cxtexto->getComment()));
 texto->setFont(fonte);
 texto->setZValue(1);
 texto->setBrush(cxtexto->getTextColor());

 //Move o texto para a posição inicial considerando os espaçamentos vertical e horizontal
 texto->setPos(HORIZ_SPACING, VERT_SPACING);

 /* Redimensiona o polígono da caixa de texto para que seu tamanho seja compatível
    com o tamanho do texto */
 this->resizePolygon(poligono, roundf(texto->boundingRect().width() + (2 * HORIZ_SPACING)),
                                       roundf(texto->boundingRect().height() + (2* VERT_SPACING)));
 caixa->setPos(0,0);
 caixa->setPolygon(poligono);

 /* Configura a posição do ícone de protegido do objeto, para que
    este esteja situado no canto inferior direito da caixa de texto */
 protected_icon->setPos(caixa->boundingRect().right() - (protected_icon->boundingRect().width() + 2 * HORIZ_SPACING),
                         caixa->boundingRect().bottom()- (protected_icon->boundingRect().height() + 2 * VERT_SPACING));

 this->bounding_rect.setTopLeft(caixa->boundingRect().topLeft());
 this->bounding_rect.setBottomRight(caixa->boundingRect().bottomRight());

 BaseObjectView::__configureObject();
 BaseObjectView::configureObjectShadow();
 BaseObjectView::configureObjectSelection();
}
