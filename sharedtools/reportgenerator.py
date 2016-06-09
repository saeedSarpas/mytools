"""reportgenerator.py
Generating report using text, pictures, tables, etc."""

import string
import random

class Report(object):
    """Handling necessary actions for generating LaTeX document"""

    def __init__(self, fname, title, **kwargs):
        """Initializing LaTeX files"""
        k = kwargs.get
        # Authors information should be passed in following format:
        styledauthors = ''
        if 'authors' in kwargs and 'emails' in kwargs:
            for author, email in zip(k('authors'), k('emails')):
                styledauthors += author + ' \thant{' + email + '}, '
        styledtitle = '\\textsc{{\\small Report}\\\\' + title + '\\vspace{8mm}}'
        self._mainfile = open(fname + '.tex', 'w')
        mwrite = self._mainfile.write
        mwrite('\\documentclass[11pt]{article}')
        mwrite('\\usepackage{amsmath}')
        mwrite('\\usepackage{geometry}')
        mwrite('\\usepackage{graphicx}')
        mwrite('\\begin{document}')
        mwrite('\t% --top matter-- %')
        mwrite('\t\\title{' + styledtitle + '}')
        mwrite('\t\\author{' + styledauthors + '}')
        mwrite('\t\\date{}')
        mwrite('\t\\maketitle')
        if 'abstract' in kwargs:
            mwrite('\t\\vspace{45mm}')
            mwrite('\t\\noindent\\begin{minipage}{0.7\\textwidth}')
            mwrite('\t\\begin{abstract}')
            mwrite(k('abstract'))
            mwrite('\\end{minipage}')
            mwrite('\\newpage')
        self.indent = '\t'

    def section(self, title, **kwargs):
        """Creating a section and putting description text (if available)"""
        k = kwargs.get
        mwrite = self._mainfile.write
        mwrite('\t\\section{' + title + '}')
        if 'text' in kwargs:
            mwrite('\t\t' + k('text'))
        self.indent = '\t\t'

    def subsection(self, title, **kwargs):
        """Creating a subsection and putting description text (if available)"""
        k = kwargs.get
        mwrite = self._mainfile.write
        mwrite('\t\t\\subsection{' + title + '}')
        if 'text' in kwargs:
            mwrite('\t\t\t' + k('text'))
        self.indent = '\t\t\t'

    def subsubsection(self, title, **kwargs):
        """Creating a subsubsection and putting description text (if available)
        """
        k = kwargs.get
        mwrite = self._mainfile.write
        mwrite('\t\t\t\\subsubsection{' + title + '}')
        if 'text' in kwargs:
            mwrite('\t\t\t\t' + k('text'))
        self.indent = '\t\t\t\t'

    def addparagraph(self, text):
        """Adding text to the document"""
        mwrite = self._mainfile.write
        mwrite(self.indent + text + '\\\\')
        mwrite('')

    def addfigure(self, path, **kwargs):
        """Adding figure to the document"""
        k = kwargs.get
        mwrite = self._mainfile.write
        specifier = k('specifier') if 'specifier' in kwargs else 'h'
        attrs = k('attrs') if 'attrs' in kwargs else ''
        mwrite(self.indent + '\\begin{figure}[' + specifier + ']')
        mwrite(self.indent + '\t\\centering')
        mwrite(self.indent + '\t\\includegraphics[' + attrs + ']{' + path + '}')
        if 'caption' in kwargs:
            mwrite(self.indent + '\t' + k('caption'))
        label = labelgenerator('fig:')
        mwrite(self.indent + '\t\\label{' + label + '}')
        mwrite(self.indent + '\\end{figure}')
        return label

    def addtableofadict(self, dictionary, **kwargs):
        """Adding a table of keys and values of a dictionary to the document"""
        k = kwargs.get
        mwrite = self._mainfile.write
        specifier = k('specifier') if 'specifier' in kwargs else 'h!'
        mwrite(self.indent + '\begin{table}[' + specifier + ']')
        mwrite(self.indent + '\t\\centering')
        mwrite(self.indent + '\t\\begin{tabular}{|l |l |}')
        mwrite(self.indent + '\t\t\\hline')
        mwrite(self.indent + '\tParameter & Value \\')
        mwrite(self.indent + '\t\t\\hline')
        for key, value in dictionary.iteritems():
            mwrite(self.indent + '\t' + str(key) + ' & ' + str(value) + '\\\\')
            mwrite(self.indent + '\t\t\\hline')
        mwrite(self.indent + '\t\\end{tabular}{|l |l |}')
        if 'caption' in kwargs:
            mwrite(self.indent + '\t' + k('caption'))
        label = labelgenerator('tab')
        mwrite(self.indent + '\t\\label{' + label + '}')
        mwrite(self.indent + '\\end{table}')

    def finish(self, **kwargs):
        """Finishing the document by adding bibliography (if available) and also
        ending the document"""
        k = kwargs.get
        mwrite = self._mainfile.write
        if 'refrences' in kwargs:
            mwrite('\t\\bibliography{' + k('refrences') + '}{}')
            mwrite('\t\\bibliographystyle{plain}')
        mwrite('\\end{document}')

def labelgenerator(prefix, size=10, chars=string.ascii_letters + string.digits):
    """Generating label for figures, tables, etc."""
    return prefix.join(random.choice(chars) for _ in range(size))
