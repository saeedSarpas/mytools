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
                styledauthors += author + ' \\thanks{' + email + '}, '
        styledtitle = '\\textsc{{\\small Report}\\\\' + title + '\\vspace{8mm}}'
        self._file = open(fname + '.tex', 'w')
        mwrite = self._file.write
        mwrite('\\documentclass[9pt]{article}\n\n')
        mwrite('\\usepackage{amsmath}\n')
        mwrite('\\usepackage{booktabs}\n')
        mwrite('\\usepackage[a4paper, top=1.2cm, right=1.5cm, bottom=2cm,' \
               'left=1cm]{geometry}\n')
        mwrite('\\usepackage{graphicx}\n\n')
        mwrite('\\begin{document}\n')
        mwrite('\t% --top matter-- %\n')
        mwrite('\t\\title{' + styledtitle + '}\n')
        mwrite('\t\\author{' + styledauthors + '}\n')
        mwrite('\t\\date{}\n')
        mwrite('\t\\maketitle\n')
        if 'abstract' in kwargs:
            mwrite('\t\\vspace{45mm}\n')
            mwrite('\t\\noindent\\begin{minipage}{0.7\\textwidth}\n')
            mwrite('\t\t\\begin{abstract}\n')
            mwrite('\t\t\t' + k('abstract') + '\n')
            mwrite('\t\t\\end{abstract}\n')
            mwrite('\t\\end{minipage}\n')
        mwrite('\t\\newpage\n')
        self.indent = '\t'

    def section(self, title, **kwargs):
        """Creating a section and putting description text (if available)"""
        k = kwargs.get
        mwrite = self._file.write
        mwrite('\t\\section{' + title + '}\n')
        if 'text' in kwargs:
            mwrite('\t\t\\noindent' + k('text') + '\\\\\n\n')
        self.indent = '\t\t'

    def subsection(self, title, **kwargs):
        """Creating a subsection and putting description text (if available)"""
        k = kwargs.get
        mwrite = self._file.write
        mwrite('\t\t\\subsection{' + title + '}\n')
        if 'text' in kwargs:
            mwrite('\t\t\t\\noindent' + k('text') + '\\\\\n\n')
        self.indent = '\t\t\t'

    def subsubsection(self, title, **kwargs):
        """Creating a subsubsection and putting description text (if available)
        """
        k = kwargs.get
        mwrite = self._file.write
        mwrite('\t\t\t\\subsubsection{' + title + '}\n')
        if 'text' in kwargs:
            mwrite('\t\t\t\t\\noindent' + k('text') + '\\\\\n\n')
        self.indent = '\t\t\t\t'

    def paragraph(self, text):
        """Adding text to the document"""
        mwrite = self._file.write
        mwrite(self.indent + text + '\\\\\n\n')

    def newpage(self):
        """Adding \newpage"""
        self._file.write(self.indent + '\\newpage\n\n')

    def figure(self, path, **kwargs):
        """Adding figure to the document"""
        k = kwargs.get
        mwrite = self._file.write
        specifier = k('specifier') if 'specifier' in kwargs else 'h'
        attrs = k('attrs') if 'attrs' in kwargs else ''
        mwrite(self.indent + '\\begin{figure}[' + specifier + ']\n')
        mwrite(self.indent + '\t\\centering\n')
        mwrite(self.indent + '\t\\includegraphics[' + attrs + ']{' + path +
               '}\n')
        if 'caption' in kwargs:
            mwrite(self.indent + '\t\\caption{' + k('caption') + '}\n')
        label = labelgenerator('fig:')
        mwrite(self.indent + '\t\\label{' + label + '}\n')
        mwrite(self.indent + '\\end{figure}\n')
        return label

    def dict2table(self, dictionary, **kwargs):
        """Adding a table of keys and values of a dictionary to the document"""
        k = kwargs.get
        mwrite = self._file.write
        specifier = k('specifier') if 'specifier' in kwargs else 'h!'
        excludekeys = k('excludekeys') if 'excludekeys' in kwargs else []
        mwrite(self.indent + '\\begin{table}[' + specifier + ']\n')
        mwrite(self.indent + '\t\\centering\n')
        mwrite(self.indent + '\t\\begin{tabular}{l l}\n')
        mwrite(self.indent + '\t\tParameter & Value \\\\\n')
        mwrite(self.indent + '\t\t\\toprule\n')
        for key, value in dictionary.iteritems():
            if str(key) not in excludekeys:
                mwrite(self.indent + '\t\t' + safetext(str(key)) + ' & ' +
                       safetext(str(value)) + '\\\\\n')
                mwrite(self.indent + '\t\t\\midrule\n')
        mwrite(self.indent + '\t\\end{tabular}\n')
        if 'caption' in kwargs:
            mwrite(self.indent + '\t' + k('caption') + '\n')
        label = labelgenerator('tab:')
        mwrite(self.indent + '\t\\label{' + label + '}\n')
        mwrite(self.indent + '\\end{table}\n')
        return label

    def finish(self, **kwargs):
        """Finishing the document by adding bibliography (if available) and also
        ending the document"""
        k = kwargs.get
        mwrite = self._file.write
        if 'refrences' in kwargs:
            mwrite('\n\t\\bibliography{' + k('refrences') + '}{}\n')
            mwrite('\t\\bibliographystyle{plain}\n\n')
        mwrite('\\end{document}\n')
        self._file.close()

def labelgenerator(prefix, size=10, chars=string.ascii_letters + string.digits):
    """Generating label for figures, tables, etc."""
    return prefix + ''.join(random.choice(chars) for _ in range(size))

def safetext(text):
    """Returning a latex safe textk"""
    return text.replace('_', '\\_').replace("'", '')
