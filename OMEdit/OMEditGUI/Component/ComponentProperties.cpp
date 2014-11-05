/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2014, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.2.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3,
 * ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */
/*
 *
 * @author Adeel Asghar <adeel.asghar@liu.se>
 *
 * RCS: $Id$
 *
 */

#include "ComponentProperties.h"

/*!
  \class Parameter
  \brief Defines one parameter. Creates name, value, unit and comment GUI controls.
  */
/*!
  \param pComponentInfo - pointer to ComponentInfo
  \param pOMCProxy - pointer to OMCProxy
  \param className - the name of the class containing the component.
  \param componentBaseClassName - the base class name of the component.
  \param componentClassName - the component class name.
  \param componentName - the name of the component.
  */
Parameter::Parameter(ComponentInfo *pComponentInfo, OMCProxy *pOMCProxy, QString className, QString componentBaseClassName,
                     QString componentClassName, QString componentName, bool inheritedComponent, QString inheritedClassName)
{
  mpNameLabel = new Label(pComponentInfo->getName());
  mpValueTextBox = new QLineEdit;
  mpValueTextBox->setMinimumWidth(100);  /* Set the minimum width for so atleast it can show the value when scroll bars are on */
  /*
    Get the value
    1.  If the component is inherited and class has extends modifier then see if the component has modifier value there.
    1.1 If the component is inherited then the rest of the case use "inheritedClassName" instead of "className". So we can get the
        default values of the modifiers that are set in the inherited class.
    2.  Check if the value is available in component modifier.
    3.  If we are fetching the modifier value of inherited class then check if the modifier value is present in extends modifier.
    4.  If no value is found then read the default value of the component.
    */
  QString value;
  QString classNameToUse = className;
  /* case 1 */
  if (inheritedComponent)
  {
    QStringList extendsModifierNames = pOMCProxy->getExtendsModifierNames(className, inheritedClassName);
    foreach (QString extendsModifierName, extendsModifierNames)
    {
      if (extendsModifierName.compare(QString(componentName).append(".").append(pComponentInfo->getName())) == 0)
      {
        value = pOMCProxy->getExtendsModifierValue(className, inheritedClassName, extendsModifierName);
      }
    }
    classNameToUse = inheritedClassName;
  }
  /* case 2 */
  if (value.isEmpty() && !componentName.isEmpty())
    value = pOMCProxy->getComponentModifierValue(classNameToUse, QString(componentName).append(".").append(pComponentInfo->getName()));
  /* case 3 */
  if (value.isEmpty() && !componentBaseClassName.isEmpty())
    value = pOMCProxy->getExtendsModifierValue(componentBaseClassName, componentClassName, pComponentInfo->getName());
  /* case 4 */
  if (value.isEmpty())
    value = pOMCProxy->getParameterValue(componentClassName, pComponentInfo->getName());
  mpValueTextBox->setText(value);
  mpValueTextBox->setCursorPosition(0); /* move the cursor to start so that parameter value will show up from start instead of end. */
  mpUnitLabel = new Label;
  mpCommentLabel = new Label;
  /*
    Get unit value
    First check if unit is defined with in the component modifier.
    If no unit is found then check it in the derived class modifier value.
    A derived class can be inherited, so look recursively.
    */
  QString unit = pOMCProxy->getComponentModifierValue(componentClassName, QString(pComponentInfo->getName()).append(".unit"));
  if (unit.isEmpty())
  {
    if (!pOMCProxy->isBuiltinType(pComponentInfo->getClassName()))
      unit = getUnitFromDerivedClass(pOMCProxy, pComponentInfo->getClassName());
  }
  mpUnitLabel = new Label(StringHandler::removeFirstLastQuotes(unit));
  mpCommentLabel = new Label(pComponentInfo->getComment());
}

/*!
  Returns the name Label.
  \return the name label.
  */
Label* Parameter::getNameLabel()
{
  return mpNameLabel;
}

/*!
  Returns the value QLineEdit.
  \return the value textbox.
  */
QLineEdit* Parameter::getValueTextBox()
{
  return mpValueTextBox;
}

/*!
  Returns the unit Label.
  \return the unit label.
  */
Label* Parameter::getUnitLabel()
{
  return mpUnitLabel;
}

/*!
  Returns the comment Label.
  \return the comment label.
  */
Label* Parameter::getCommentLabel()
{
  return mpCommentLabel;
}

/*!
  Returns the unit value by reading the derived classes.
  \return the unit value.
  */
QString Parameter::getUnitFromDerivedClass(OMCProxy *pOMCProxy, QString className)
{
  int inheritanceCount = pOMCProxy->getInheritanceCount(className);
  if (inheritanceCount == 0)
  {
    return pOMCProxy->getDerivedClassModifierValue(className, "unit");
  }
  else
  {
    for(int i = 1 ; i <= inheritanceCount ; i++)
    {
      QString inheritedClass = pOMCProxy->getNthInheritedClass(className, i);
      if (pOMCProxy->isBuiltinType(inheritedClass))
        return pOMCProxy->getDerivedClassModifierValue(className, "unit");
      if (inheritedClass.compare(className) != 0)
        return getUnitFromDerivedClass(pOMCProxy, inheritedClass);
    }
  }
  return "";
}

/*!
  Sets the input field of the parameter enable/disable.
  \param enable
  */
void Parameter::setEnabled(bool enable)
{
  mpValueTextBox->setEnabled(enable);
}

/*!
  \class GroupBox
  \brief Creates a group for parameters.
  */
GroupBox::GroupBox(const QString &title, QWidget *parent)
  : QGroupBox(title, parent)
{
  mpGroupImageLabel = new Label;
  mpGridLayout = new QGridLayout;
  mpGridLayout->setObjectName(title);
  mpGridLayout->setAlignment(Qt::AlignLeft);
  mpHorizontalLayout = new QHBoxLayout;
  mpHorizontalLayout->setAlignment(Qt::AlignLeft);
  mpHorizontalLayout->addLayout(mpGridLayout, 1);
  mpHorizontalLayout->addWidget(mpGroupImageLabel, 0, Qt::AlignRight);
  setLayout(mpHorizontalLayout);
}

/*!
  Sets the group image.
  \param groupImage - the absolute path of the image.
  */
void GroupBox::setGroupImage(QString groupImage)
{
  if (!mpGroupImageLabel->pixmap() || (mpGroupImageLabel->pixmap() && mpGroupImageLabel->pixmap()->isNull())) {
    QPixmap pixmap(groupImage);
    mpGroupImageLabel->setPixmap(pixmap);
  }
}

/*!
  \class ParametersScrollArea
  \brief Creates a scroll area for each tab of the component parameters dialog.
  */
ParametersScrollArea::ParametersScrollArea()
{
  mpWidget = new QWidget;
  setFrameShape(QFrame::NoFrame);
  setBackgroundRole(QPalette::Base);
  setWidgetResizable(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  mpVerticalLayout = new QVBoxLayout;
  mpVerticalLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  mpWidget->setLayout(mpVerticalLayout);
  setWidget(mpWidget);
}

/*!
 * Override eventFilter
 * No vertical scroll bar in dialogs with long parameter explanation
 */
bool ParametersScrollArea::eventFilter(QObject *o, QEvent *e)
{
  if(o == mpWidget && e->type() == QEvent::Resize)
    setMinimumHeight(mpWidget->minimumSizeHint().height() + horizontalScrollBar()->height());

  return false;
}

/*!
  Adds a QGroupBox to the layout.
  \param pGroupBox - pointer to QGroupBox.
  \param pGroupBoxLayout - pointer to QGridLayout.
  */
void ParametersScrollArea::addGroupBox(GroupBox *pGroupBox)
{
  if (!getGroupBox(pGroupBox->title()))
  {
    pGroupBox->hide();  /* create a hidden groupbox, we show it when it contains the parameters. */
    mGroupBoxesList.append(pGroupBox);
    mpVerticalLayout->addWidget(pGroupBox);
  }
}

/*!
  Returns the GroupBox by reading the list of GroupBoxes.
  \return the GroupBox
  */
GroupBox* ParametersScrollArea::getGroupBox(QString title)
{
  foreach (GroupBox *pGroupBox, mGroupBoxesList)
  {
    if (pGroupBox->title().compare(title) == 0)
      return pGroupBox;
  }
  return 0;
}

/*!
  Returns the main layout of the widget.
  \return the QVBoxLayout
  */
QVBoxLayout *ParametersScrollArea::getLayout()
{
  return mpVerticalLayout;
}

/*!
  \class ComponentParameters
  \brief A dialog for displaying components parameters.
  */
/*!
  \param parametersOnly - flag true => only collect parameters info, false => collect all variables.
  \param pComponent - pointer to Component
  \param pMainWindow - pointer to MainWindow
  */
ComponentParameters::ComponentParameters(Component *pComponent, MainWindow *pMainWindow)
  : QDialog(pMainWindow, Qt::WindowTitleHint)
{
  setWindowTitle(QString(Helper::applicationName).append(" - ").append(tr("Component Parameters")));
  setAttribute(Qt::WA_DeleteOnClose);
  mpComponent = pComponent;
  mpMainWindow = pMainWindow;
  setUpDialog();
}

/*!
  Deletes the list of Parameter objects
  */
ComponentParameters::~ComponentParameters()
{
  qDeleteAll(mParametersList.begin(), mParametersList.end());
  mParametersList.clear();
}

/*!
  Creates the Dialog and set up all the controls with default values.
  */
void ComponentParameters::setUpDialog()
{
  // heading label
  mpParametersHeading = new Label(Helper::parameters);
  mpParametersHeading->setFont(QFont(Helper::systemFontInfo.family(), Helper::headingFontSize));
  mpParametersHeading->setAlignment(Qt::AlignTop);
  // set seperator line
  mHorizontalLine = new QFrame();
  mHorizontalLine->setFrameShape(QFrame::HLine);
  mHorizontalLine->setFrameShadow(QFrame::Sunken);
  // parameters tab widget
  mpParametersTabWidget = new QTabWidget;
  // Component Group Box
  mpComponentGroupBox = new QGroupBox(tr("Component"));
  // Component name
  mpComponentNameLabel = new Label(Helper::name);
  mpComponentNameTextBox = new Label(mpComponent->getName());
  // Component class name
  mpComponentClassNameLabel = new Label(Helper::path);
  mpComponentClassNameTextBox = new Label(mpComponent->getClassName());
  QGridLayout *pComponentGroupBoxLayout = new QGridLayout;
  pComponentGroupBoxLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  pComponentGroupBoxLayout->addWidget(mpComponentNameLabel, 0, 0);
  pComponentGroupBoxLayout->addWidget(mpComponentNameTextBox, 0, 1);
  pComponentGroupBoxLayout->addWidget(mpComponentClassNameLabel, 1, 0);
  pComponentGroupBoxLayout->addWidget(mpComponentClassNameTextBox, 1, 1);
  mpComponentGroupBox->setLayout(pComponentGroupBoxLayout);
  // Create General tab and Parameters GroupBox
  ParametersScrollArea *pParametersScrollArea = new ParametersScrollArea;
  // first add the Component Group Box
  pParametersScrollArea->getLayout()->addWidget(mpComponentGroupBox);
  GroupBox *pGroupBox = new GroupBox("Parameters");
  pParametersScrollArea->addGroupBox(pGroupBox);
  mTabsMap.insert("General", mpParametersTabWidget->addTab(pParametersScrollArea, "General"));
  // create parameters tabs and groupboxes
  createTabsAndGroupBoxes(mpComponent->getOMCProxy(), mpComponent->getClassName());
  // create the parameters controls
  createParameters(mpComponent->getOMCProxy(), mpComponent->getGraphicsView()->getModelWidget()->getLibraryTreeNode()->getNameStructure(),
                   "", mpComponent->getClassName(), mpComponent->getName(), mpComponent->isInheritedComponent(),
                   mpComponent->getInheritedClassName());
  // create Modifiers tab
  QWidget *pModifiersTab = new QWidget;
  // add items to modifiers tab
  mpModifiersLabel = new Label(tr("Add new modifiers, e.g phi(start=1),w(start=2)"));
  mpModifiersTextBox = new QLineEdit;
  QVBoxLayout *pModifiersTabLayout = new QVBoxLayout;
  pModifiersTabLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  pModifiersTabLayout->addWidget(mpModifiersLabel);
  pModifiersTabLayout->addWidget(mpModifiersTextBox);
  pModifiersTab->setLayout(pModifiersTabLayout);
  mpParametersTabWidget->addTab(pModifiersTab, "Modifiers");
  // Create the buttons
  mpOkButton = new QPushButton(Helper::ok);
  mpOkButton->setAutoDefault(true);
  connect(mpOkButton, SIGNAL(clicked()), this, SLOT(updateComponentParameters()));
  if (mpComponent->getGraphicsView()->getModelWidget()->getLibraryTreeNode()->isSystemLibrary())
    mpOkButton->setDisabled(true);
  mpCancelButton = new QPushButton(Helper::cancel);
  mpCancelButton->setAutoDefault(false);
  connect(mpCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  mpButtonBox = new QDialogButtonBox(Qt::Horizontal);
  mpButtonBox->addButton(mpOkButton, QDialogButtonBox::ActionRole);
  mpButtonBox->addButton(mpCancelButton, QDialogButtonBox::ActionRole);
  // main layout
  QVBoxLayout *pMainLayout = new QVBoxLayout;
  pMainLayout->addWidget(mpParametersHeading);
  pMainLayout->addWidget(mHorizontalLine);
  pMainLayout->addWidget(mpParametersTabWidget);
  pMainLayout->addWidget(mpButtonBox);
  setLayout(pMainLayout);
}

/*!
  Reads the component's annotations and creates the dynamic tabs for QTabWidget and QGroupBoxes with in them.
  It goes recursively into the inherited classes of the component and read all the parameters in them.
  \param pOMCProxy - pointer to OMCProxy
  \param componentClassName - the name of the component class.
  */
void ComponentParameters::createTabsAndGroupBoxes(OMCProxy *pOMCProxy, QString componentClassName, QString componentBaseClassName)
{
  int i = 0;
  QList<ComponentInfo*> componentInfoList = pOMCProxy->getComponents(componentClassName);
  QStringList componentAnnotations = pOMCProxy->getComponentAnnotations(componentClassName);
  foreach (ComponentInfo *pComponentInfo, componentInfoList)
  {
    /*
      Ticket #2531
      Do not show the protected & final parameters.
      */
    if (pComponentInfo->getProtected() || pComponentInfo->getFinal())
    {
      i++;
      continue;
    }
    /*
      Ticket #2531
      Check if parameter is marked final in the extends modifier.
      */
    if ((!componentBaseClassName.isEmpty()) && pOMCProxy->isExtendsModifierFinal(componentBaseClassName, componentClassName, pComponentInfo->getName()))
    {
      i++;
      continue;
    }
    /*
      I didn't find anything useful in the specification regarding this issue.
      The parameters dialog is only suppose to show the parameters. However, Dymola also shows the variables in the parameters window
      which have the dialog annotation with them. So, if the variable has dialog annotation or it is a parameter then show it.
      */
    QString tab = "";
    QString groupBox = "";
    QStringList dialogAnnotation = StringHandler::getDialogAnnotation(componentAnnotations[i]);
    if ((pComponentInfo->getVariablity().compare("parameter") == 0) || (dialogAnnotation.size() > 0))
    {
      if (dialogAnnotation.size() > 0)
      {
        // get the tab value
        tab = StringHandler::removeFirstLastQuotes(dialogAnnotation.at(0));
        // get the group value
        groupBox = StringHandler::removeFirstLastQuotes(dialogAnnotation.at(1));
        if (!mTabsMap.contains(tab))
        {
          ParametersScrollArea *pParametersScrollArea = new ParametersScrollArea;
          GroupBox *pGroupBox = new GroupBox(groupBox);
          pParametersScrollArea->addGroupBox(pGroupBox);
          mTabsMap.insert(tab, mpParametersTabWidget->addTab(pParametersScrollArea, tab));
        }
        else
        {
          ParametersScrollArea *pParametersScrollArea;
          pParametersScrollArea = qobject_cast<ParametersScrollArea*>(mpParametersTabWidget->widget(mTabsMap.value(tab)));
          if (pParametersScrollArea)
          {
            if (!pParametersScrollArea->getGroupBox(groupBox))
            {
              GroupBox *pGroupBox = new GroupBox(groupBox);
              pParametersScrollArea->addGroupBox(pGroupBox);
            }
          }
        }
      }
    }
    i++;
  }
  int inheritanceCount = pOMCProxy->getInheritanceCount(componentClassName);
  for(int i = 1 ; i <= inheritanceCount ; i++)
  {
    QString inheritedClass = pOMCProxy->getNthInheritedClass(componentClassName, i);
    if (!pOMCProxy->isBuiltinType(inheritedClass) && inheritedClass.compare(componentClassName) != 0)
      createTabsAndGroupBoxes(pOMCProxy, inheritedClass, componentClassName);
  }
}

/*!
  Reads the component's parameters and creates the dynamic GUI controls for it.\n
  It goes recursively into the inherited classes of the component and read all the parameters in them.
  \param pOMCProxy - pointer to OMCProxy
  \param className - the name of the class containing the component.
  \param componentBaseClassName - the name of the base class name of the component.
  \param componentClassName - the name of the component class.
  \param componentName - the componentName.
  \param layoutIndex - the index value of the layout, tells the layout where to put the parameter GUI controls.
  */
void ComponentParameters::createParameters(OMCProxy *pOMCProxy, QString className, QString componentBaseClassName,
                                           QString componentClassName, QString componentName, bool inheritedComponent,
                                           QString inheritedClassName, bool isInheritedCycle)
{
  int i = -1;
  QList<ComponentInfo*> componentInfoList = pOMCProxy->getComponents(componentClassName);
  QStringList componentAnnotations = pOMCProxy->getComponentAnnotations(componentClassName);
  foreach (ComponentInfo *pComponentInfo, componentInfoList)
  {
    i++;
    /*
      Ticket #2531
      Do not show the protected & final parameters.
      */
    if (pComponentInfo->getProtected() || pComponentInfo->getFinal())
      continue;
    /*
      Ticket #2531
      Check if parameter is marked final in the extends modifier.
      */
    if ((!componentBaseClassName.isEmpty()) && pOMCProxy->isExtendsModifierFinal(componentBaseClassName, componentClassName, pComponentInfo->getName()))
      continue;
    QString tab = QString("General");
    QString groupBox = QString("Parameters");
    bool enable = true;
    QString groupImage = "";
    QStringList dialogAnnotation = StringHandler::getDialogAnnotation(componentAnnotations[i]);
    if ((pComponentInfo->getVariablity().compare("parameter") == 0) || (dialogAnnotation.size() > 0))
    {
      if (dialogAnnotation.size() > 0)
      {
        // get the tab value
        tab = StringHandler::removeFirstLastQuotes(dialogAnnotation.at(0));
        // get the group value
        groupBox = StringHandler::removeFirstLastQuotes(dialogAnnotation.at(1));
        // get the enable value
        enable = dialogAnnotation.at(2).contains("true");
        // get the group image
        groupImage = StringHandler::removeFirstLastQuotes(dialogAnnotation.at(9));
        groupImage = mpMainWindow->getOMCProxy()->uriToFilename(groupImage);
      }
      ParametersScrollArea *pParametersScrollArea;
      pParametersScrollArea = qobject_cast<ParametersScrollArea*>(mpParametersTabWidget->widget(mTabsMap.value(tab)));
      if (pParametersScrollArea)
      {
        GroupBox *pGroupBox = pParametersScrollArea->getGroupBox(groupBox);
        if (pGroupBox)
        {
          // set the group image
          pGroupBox->setGroupImage(groupImage);
          QGridLayout *pGroupBoxGridLayout = pGroupBox->getGridLayout();
          int layoutIndex = pGroupBoxGridLayout->rowCount();
          /* We hide the groupbox when we create it. Show the groupbox now since it has a parameter. */
          pGroupBox->show();
          /* create a parameter */
          Parameter *pParameter = new Parameter(pComponentInfo, pOMCProxy, className, componentBaseClassName, componentClassName,
                                                componentName, inheritedComponent, inheritedClassName);
          pParameter->setEnabled(enable);
          /*
            Show a line under the inherited parameter and display the name of the
            */
          if (isInheritedCycle)
          {
            /*
              border-bottom doesn't work correctly. So set the border on all sides and then remove it from left, top and right.
              */
            pParameter->getNameLabel()->setStyleSheet("QLabel{border:1px dotted #000; border-left:none; border-top:none; border-right:none;}");
            pParameter->getNameLabel()->setToolTip(tr("Inherited from <b>%1</b>").arg(componentClassName));
          }
          pGroupBoxGridLayout->addWidget(pParameter->getNameLabel(), layoutIndex, 0);
          if (dialogAnnotation.size() > 3)
          {
            if (dialogAnnotation.at(2).compare("false") == 0)
              pParameter->getValueTextBox()->setEnabled(false);
          }
          pGroupBoxGridLayout->addWidget(pParameter->getValueTextBox(), layoutIndex, 1);
          pGroupBoxGridLayout->addWidget(pParameter->getUnitLabel(), layoutIndex, 2);
          pGroupBoxGridLayout->addWidget(pParameter->getCommentLabel(), layoutIndex, 3);
          mParametersList.append(pParameter);
        }
      }
    }
  }
  int inheritanceCount = pOMCProxy->getInheritanceCount(componentClassName);
  for(int i = 1 ; i <= inheritanceCount ; i++)
  {
    QString inheritedClass = pOMCProxy->getNthInheritedClass(componentClassName, i);
    if (!pOMCProxy->isBuiltinType(inheritedClass) && inheritedClass.compare(componentClassName) != 0)
    {
      createParameters(pOMCProxy, className, componentClassName, inheritedClass, componentName, inheritedComponent,
                       inheritedClassName, true);
    }
  }
}

/*!
  Returns the parameters list.
  \return the list of parameters.
  */
QList<Parameter*> ComponentParameters::getParametersList()
{
  return mParametersList;
}

/*!
  Slot activated when mpOkButton clicked signal is raised.\n
  Checks the list of parameters i.e mParametersList and if the value is changed then sets the new value.
  */
void ComponentParameters::updateComponentParameters()
{
  bool valueChanged = false;
  bool modifierValueChanged = false;
  foreach (Parameter *pParameter, mParametersList)
  {
    QLineEdit *pValueTextBox = pParameter->getValueTextBox();
    if (pValueTextBox->isModified())
    {
      valueChanged = true;
      QString className = mpComponent->getGraphicsView()->getModelWidget()->getLibraryTreeNode()->getNameStructure();
      QString componentModifier = QString(mpComponent->getName()).append(".").append(pParameter->getNameLabel()->text());
      QString componentModifierValue = pValueTextBox->text().trimmed();
      /* If the component is inherited then add the modifier value into the extends. */
      if (mpComponent->isInheritedComponent())
      {
        if (mpComponent->getOMCProxy()->setExtendsModifierValue(className, mpComponent->getInheritedClassName(), componentModifier, componentModifierValue.prepend("=")))
          modifierValueChanged = true;
      }
      else
      {
        if (mpComponent->getOMCProxy()->setComponentModifierValue(className, componentModifier, componentModifierValue.prepend("=")))
          modifierValueChanged = true;
      }
    }
  }
  // add modifiers
  if (!mpModifiersTextBox->text().isEmpty())
  {
    QString regexp ("([A-Za-z0-9]+)\\(([A-Za-z0-9]+)=([A-Za-z0-9]+)\\)$");
    QRegExp modifierRegExp (regexp);
    QStringList modifiers = mpModifiersTextBox->text().split(",", QString::SkipEmptyParts);
    foreach (QString modifier, modifiers)
    {
      modifier = modifier.trimmed();
      if (modifierRegExp.exactMatch(modifier.trimmed()))
      {
        QString className = mpComponent->getGraphicsView()->getModelWidget()->getLibraryTreeNode()->getNameStructure();
        QString componentModifier = QString(mpComponent->getName()).append(".").append(modifier.mid(0, modifier.indexOf("(")));
        QString componentModifierValue = modifier.mid(modifier.indexOf("("));
        mpComponent->getOMCProxy()->setComponentModifierValue(className, componentModifier, componentModifierValue);
        valueChanged = true;
      }
      else
      {
        mpMainWindow->getMessagesWidget()->addGUIMessage(new MessagesTreeItem("", false, 0, 0, 0, 0,
                                                                              GUIMessages::getMessage(GUIMessages::WRONG_MODIFIER).arg(modifier),
                                                                              Helper::scriptingKind, Helper::errorLevel, 0,
                                                                              mpMainWindow->getMessagesWidget()->getMessagesTreeWidget()));
      }
    }
  }
  // if valueChanged is true then set the model modified.
  if (valueChanged)
    mpComponent->getGraphicsView()->getModelWidget()->setModelModified();
  if (modifierValueChanged)
    mpComponent->componentParameterHasChanged();
  accept();
}

/*!
  \class ComponentAttributes
  \brief A dialog for displaying components attributes like visibility, stream, casuality etc.
  */
/*!
  \param pComponent - pointer to Component
  \param pMainWindow - pointer to MainWindow
  */
ComponentAttributes::ComponentAttributes(Component *pComponent, MainWindow *pMainWindow)
  : QDialog(pMainWindow, Qt::WindowTitleHint)
{
  setWindowTitle(QString(Helper::applicationName).append(" - ").append(tr("Component Attributes")));
  setAttribute(Qt::WA_DeleteOnClose);
  mpComponent = pComponent;
  mpMainWindow = pMainWindow;
  setUpDialog();
  initializeDialog();
}

/*!
  Creates the Dialog and set up all the controls with default values.
  */
void ComponentAttributes::setUpDialog()
{
  // heading label
  mpAttributesHeading = new Label(Helper::attributes);
  mpAttributesHeading->setFont(QFont("", Helper::headingFontSize));
  mpAttributesHeading->setAlignment(Qt::AlignTop);
  // set seperator line
  mHorizontalLine = new QFrame();
  mHorizontalLine->setFrameShape(QFrame::HLine);
  mHorizontalLine->setFrameShadow(QFrame::Sunken);
  // create Type Group Box
  mpTypeGroupBox = new QGroupBox(Helper::type);
  QGridLayout *pTypeGroupBoxLayout = new QGridLayout;
  mpNameLabel = new Label(Helper::name);
  mpNameTextBox = new QLineEdit;
  mpCommentLabel = new Label(Helper::comment);
  mpCommentTextBox = new QLineEdit;
  mpPathLabel = new Label(Helper::path);
  mpPathTextBox = new Label;
  pTypeGroupBoxLayout->addWidget(mpNameLabel, 0, 0);
  pTypeGroupBoxLayout->addWidget(mpNameTextBox, 0, 1);
  pTypeGroupBoxLayout->addWidget(mpCommentLabel, 1, 0);
  pTypeGroupBoxLayout->addWidget(mpCommentTextBox, 1, 1);
  pTypeGroupBoxLayout->addWidget(mpPathLabel, 2, 0);
  pTypeGroupBoxLayout->addWidget(mpPathTextBox, 2, 1);
  mpTypeGroupBox->setLayout(pTypeGroupBoxLayout);
  // create Variablity Group Box
  mpVariabilityGroupBox = new QGroupBox("Variability");
  mpConstantRadio = new QRadioButton("Constant");
  mpParameterRadio = new QRadioButton("Parameter");
  mpDiscreteRadio = new QRadioButton("Discrete");
  mpDefaultRadio = new QRadioButton("Unspecified (Default)");
  QVBoxLayout *pVariabilityGroupBoxLayout = new QVBoxLayout;
  pVariabilityGroupBoxLayout->addWidget(mpConstantRadio);
  pVariabilityGroupBoxLayout->addWidget(mpParameterRadio);
  pVariabilityGroupBoxLayout->addWidget(mpDiscreteRadio);
  pVariabilityGroupBoxLayout->addWidget(mpDefaultRadio);
  mpVariabilityButtonGroup = new QButtonGroup;
  mpVariabilityButtonGroup->addButton(mpConstantRadio);
  mpVariabilityButtonGroup->addButton(mpParameterRadio);
  mpVariabilityButtonGroup->addButton(mpDiscreteRadio);
  mpVariabilityButtonGroup->addButton(mpDefaultRadio);
  mpVariabilityGroupBox->setLayout(pVariabilityGroupBoxLayout);
  // create Variablity Group Box
  mpPropertiesGroupBox = new QGroupBox("Properties");
  mpFinalCheckBox = new QCheckBox("Final");
  mpProtectedCheckBox = new QCheckBox("Protected");
  mpReplaceAbleCheckBox = new QCheckBox("Replaceable");
  QVBoxLayout *pPropertiesGroupBoxLayout = new QVBoxLayout;
  pPropertiesGroupBoxLayout->addWidget(mpFinalCheckBox);
  pPropertiesGroupBoxLayout->addWidget(mpProtectedCheckBox);
  pPropertiesGroupBoxLayout->addWidget(mpReplaceAbleCheckBox);
  mpPropertiesGroupBox->setLayout(pPropertiesGroupBoxLayout);
  // create Variablity Group Box
  mpCausalityGroupBox = new QGroupBox("Causality");
  mpInputRadio = new QRadioButton("Input");
  mpOutputRadio = new QRadioButton("Output");
  mpNoneRadio = new QRadioButton("None");
  QVBoxLayout *pCausalityGroupBoxLayout = new QVBoxLayout;
  pCausalityGroupBoxLayout->addWidget(mpInputRadio);
  pCausalityGroupBoxLayout->addWidget(mpOutputRadio);
  pCausalityGroupBoxLayout->addWidget(mpNoneRadio);
  mpCausalityButtonGroup = new QButtonGroup;
  mpCausalityButtonGroup->addButton(mpInputRadio);
  mpCausalityButtonGroup->addButton(mpOutputRadio);
  mpCausalityButtonGroup->addButton(mpNoneRadio);
  mpCausalityGroupBox->setLayout(pCausalityGroupBoxLayout);
  // create Variablity Group Box
  mpInnerOuterGroupBox = new QGroupBox("Inner/Outer");
  mpInnerCheckBox = new QCheckBox("Inner");
  mpOuterCheckBox = new QCheckBox("Outer");
  QVBoxLayout *pInnerOuterGroupBoxLayout = new QVBoxLayout;
  pInnerOuterGroupBoxLayout->addWidget(mpInnerCheckBox);
  pInnerOuterGroupBoxLayout->addWidget(mpOuterCheckBox);
  mpInnerOuterGroupBox->setLayout(pInnerOuterGroupBoxLayout);
  // Create the buttons
  mpOkButton = new QPushButton(Helper::ok);
  mpOkButton->setAutoDefault(true);
  connect(mpOkButton, SIGNAL(clicked()), this, SLOT(updateComponentAttributes()));
  mpCancelButton = new QPushButton(Helper::cancel);
  mpCancelButton->setAutoDefault(false);
  connect(mpCancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  mpButtonBox = new QDialogButtonBox(Qt::Horizontal);
  mpButtonBox->addButton(mpOkButton, QDialogButtonBox::ActionRole);
  if (mpComponent->getGraphicsView()->getModelWidget()->getLibraryTreeNode()->isSystemLibrary() || mpComponent->isInheritedComponent())
    mpOkButton->setDisabled(true);
  mpButtonBox->addButton(mpCancelButton, QDialogButtonBox::ActionRole);
  // Create a layout
  QGridLayout *pMainLayout = new QGridLayout;
  pMainLayout->setAlignment(Qt::AlignTop);
  pMainLayout->addWidget(mpAttributesHeading, 0, 0, 1, 2);
  pMainLayout->addWidget(mHorizontalLine, 1, 0, 1, 2);
  pMainLayout->addWidget(mpTypeGroupBox, 2, 0, 1, 2);
  pMainLayout->addWidget(mpVariabilityGroupBox, 3, 0);
  pMainLayout->addWidget(mpPropertiesGroupBox, 3, 1);
  pMainLayout->addWidget(mpCausalityGroupBox, 4, 0);
  pMainLayout->addWidget(mpInnerOuterGroupBox, 4, 1);
  pMainLayout->addWidget(mpButtonBox, 5, 0, 1, 2);
  setLayout(pMainLayout);
}

/*!
  Initialize the fields with default values.
  */
void ComponentAttributes::initializeDialog()
{
  QString className;
  if (mpComponent->isInheritedComponent())
    className = mpComponent->getInheritedClassName();
  else
    className = mpComponent->getGraphicsView()->getModelWidget()->getLibraryTreeNode()->getNameStructure();
  /* get the components of the class */
  QList<ComponentInfo*> componentInfoList = mpComponent->getOMCProxy()->getComponents(className);
  /* read the components */
  foreach (ComponentInfo *pComponentInfo, componentInfoList)
  {
    if (pComponentInfo->getName() == mpComponent->getName())
    {
      mpComponentInfo = pComponentInfo;
      // get Class Name
      mpNameTextBox->setText(pComponentInfo->getName());
      mpNameTextBox->setCursorPosition(0);
      // get Comment
      mpCommentTextBox->setText(pComponentInfo->getComment());
      mpCommentTextBox->setCursorPosition(0);
      // get classname
      mpPathTextBox->setText(pComponentInfo->getClassName());
      // get Variability
      if (pComponentInfo->getVariablity() == "constant")
        mpConstantRadio->setChecked(true);
      else if (pComponentInfo->getVariablity() == "parameter")
        mpParameterRadio->setChecked(true);
      else if (pComponentInfo->getVariablity() == "discrete")
        mpDiscreteRadio->setChecked(true);
      else
        mpDefaultRadio->setChecked(true);
      // get Properties
      mpFinalCheckBox->setChecked(pComponentInfo->getFinal());
      mpProtectedCheckBox->setChecked(pComponentInfo->getProtected());
      mpReplaceAbleCheckBox->setChecked(pComponentInfo->getReplaceable());
      mIsFlow = pComponentInfo->getFlow() ? "true" : "false";
      // get Casuality
      if (pComponentInfo->getCasuality() == "input")
        mpInputRadio->setChecked(true);
      else if (pComponentInfo->getCasuality() == "output")
        mpOutputRadio->setChecked(true);
      else
        mpNoneRadio->setChecked(true);
      // get InnerOuter
      mpInnerCheckBox->setChecked(pComponentInfo->getInner());
      mpOuterCheckBox->setChecked(pComponentInfo->getOuter());
      break;
    }
  }
}

/*!
  Slot activated when mpOkButton clicked signal is raised.\n
  Updates the component attributes.
  */
void ComponentAttributes::updateComponentAttributes()
{
  if (!mpComponentInfo)
  {
    accept();
    return;
  }
  ModelWidget *pModelWidget = mpComponent->getGraphicsView()->getModelWidget();
  /* Check the same component name problem before setting any attributes. */
  if (mpComponentInfo->getName().compare(mpNameTextBox->text()) != 0)
  {
    if (!mpComponent->getGraphicsView()->checkComponentName(mpNameTextBox->text()))
    {
      QMessageBox::information(pModelWidget->getModelWidgetContainer()->getMainWindow(),
                               QString(Helper::applicationName).append(" - ").append(Helper::information),
                               GUIMessages::getMessage(GUIMessages::SAME_COMPONENT_NAME), Helper::ok);
      return;
    }
  }
  QString modelName = pModelWidget->getLibraryTreeNode()->getNameStructure();
  QString isFinal = mpFinalCheckBox->isChecked() ? "true" : "false";
  QString isProtected = mpProtectedCheckBox->isChecked() ? "true" : "false";
  QString isReplaceAble = mpReplaceAbleCheckBox->isChecked() ? "true" : "false";
  QString variability;
  if (mpConstantRadio->isChecked())
    variability = "constant";
  else if (mpParameterRadio->isChecked())
    variability = "parameter";
  else if (mpDiscreteRadio->isChecked())
    variability = "discrete";
  else
    variability = "";
  QString isInner = mpInnerCheckBox->isChecked() ? "true" : "false";
  QString isOuter = mpOuterCheckBox->isChecked() ? "true" : "false";
  QString causality;
  if (mpInputRadio->isChecked())
    causality = "input";
  else if (mpOutputRadio->isChecked())
    causality = "output";
  else
    causality = "";

  OMCProxy *pOMCProxy = pModelWidget->getModelWidgetContainer()->getMainWindow()->getOMCProxy();
  // update component attributes
  if (!pOMCProxy->setComponentProperties(modelName, mpComponentInfo->getName(), isFinal, mIsFlow, isProtected, isReplaceAble, variability,
                                         isInner, isOuter, causality))
  {
    QMessageBox::critical(pModelWidget->getModelWidgetContainer()->getMainWindow(),
                          QString(Helper::applicationName).append(" - ").append(Helper::error), pOMCProxy->getResult(), Helper::ok);
    pOMCProxy->printMessagesStringInternal();
  }
  // update the component comment only if its changed.
  if (mpComponentInfo->getComment().compare(mpCommentTextBox->text()) != 0)
  {
    QString comment = StringHandler::escapeString(mpCommentTextBox->text());
    if (!pOMCProxy->setComponentComment(modelName, mpComponentInfo->getName(), comment))
    {
      QMessageBox::critical(pModelWidget->getModelWidgetContainer()->getMainWindow(),
                            QString(Helper::applicationName).append(" - ").append(Helper::error), pOMCProxy->getResult(), Helper::ok);
      pOMCProxy->printMessagesStringInternal();
    }
  }
  // update the component name only if its changed.
  if (mpComponentInfo->getName().compare(mpNameTextBox->text()) != 0)
  {
    // if renameComponentInClass command is successful update the component with new name
    if (pOMCProxy->renameComponentInClass(modelName, mpComponentInfo->getName(), mpNameTextBox->text()))
    {
      mpComponent->componentNameHasChanged(mpNameTextBox->text());
    }
    else
    {
      QMessageBox::critical(pModelWidget->getModelWidgetContainer()->getMainWindow(),
                            QString(Helper::applicationName).append(" - ").append(Helper::error), pOMCProxy->getResult(), Helper::ok);
      pOMCProxy->printMessagesStringInternal();
    }
  }
  mpComponent->getGraphicsView()->getModelWidget()->setModelModified();
  accept();
}
