TEMPLATE = subdirs

SUBDIRS = QtDesignerPlugin QtJsonEditor QtJsonEditorDialog QtJsonWidget

QtJsonEditor.depends = QtJsonEditorDialog QtJsonWidget
QtDesignerPlugin.depends = QtJsonWidget
