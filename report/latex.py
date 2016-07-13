"""latex.py
Generating a latex document using text, pictures, tables, etc."""

import string
import random

class LaTeX(object):
    """Generate a LaTeX report

    Parameters
    ----------
    fname : str
        The name of the latex file
    title : str
        Title of the report
    authors : list of str, optional
        Authors names
    emails : list of str, optional
        Authors emails
    abstract : str

    Methods
    -------
    section(title, **kwargs)
        Creating a new section
    subsection(title, **kwargs)
        Creating a new subsection
    subsubsection(title, **kwargs)
        Creating a new subsubsection
    paragraph(text)
        Add a new paragraph
    newpage()
        Add a newpage tag
    figure(path, **kwargs)
        Add a new figure
    dict2table(dictionary, **kwargs)
        Add a table based on a given dictionary
    finish(**kwargs)
        Finalizing the report

    Examples
    --------
    >>> from mytools.report.latex import LaTeX
    >>> report = LaTeX(
            '/path/to/the/output/file',
            '<Report Title>',
            authors=['John Doe'],
            emails=['john@doe.com'])
    """

    def __init__(self, fname, title, **kwargs):
        """Initializing LaTeX files"""

        k = kwargs.get

        styledauthors = ''
        if 'authors' in kwargs and 'emails' in kwargs:
            for author, email in zip(k('authors'), k('emails')):
                styledauthors += author + ' \\thanks{' + email + '}, '
        styledauthors = styledauthors[:-2]

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
        """Adding a section and its description (if available)

        Parameters
        ----------
        title : str
            Title of the section
        text : str, optional
            Descriptio of the section

        Examples
        --------
        >>> report.section(
                '<Section Title>',
                text='Lorem ipsum dolor sit amet, consectetur adipiscing.')
        """
        mwrite = self._file.write

        mwrite('\t\\section{' + title + '}\n')

        if 'text' in kwargs:
            mwrite('\t\t\\noindent' + kwargs.get('text') + '\\\\\n\n')

        self.indent = '\t\t'

    def subsection(self, title, **kwargs):
        """Adding a subsection and its description (if available)

        Parameters
        ----------
        title : str
            Title of the subsection
        text : str, optional
            Descriptio of the subsection

        Examples
        --------
        >>> report.subsection(
                '<Subsection Title>',
                text='Lorem ipsum dolor sit amet, consectetur adipiscing.')
        """

        mwrite = self._file.write

        mwrite('\t\t\\subsection{' + title + '}\n')

        if 'text' in kwargs:
            mwrite('\t\t\t\\noindent' + kwargs.get('text') + '\\\\\n\n')

        self.indent = '\t\t\t'

    def subsubsection(self, title, **kwargs):
        """Adding a subsubsection and its description (if available)

        Parameters
        ----------
        title : str
            Title of the subsubsection
        text : str, optional
            Descriptio of the subsubsection

        Examples
        --------
        >>> report.subsubsection(
                '<Subsubsection Title>',
                text='Lorem ipsum dolor sit amet, consectetur adipiscing.')
        """

        mwrite = self._file.write

        mwrite('\t\t\t\\subsubsection{' + title + '}\n')

        if 'text' in kwargs:
            mwrite('\t\t\t\t\\noindent' + kwargs.get('text') + '\\\\\n\n')

        self.indent = '\t\t\t\t'

    def paragraph(self, text):
        """Adding a paragraph to the document

        Parameters
        ----------
        text : str
            Paragraph body text

        Examples
        --------
        >>> report.paragraph(
                'Duis aute irure dolor in reprehenderit in voluptate velit esse
                cillum dolore eu fugiat nulla pariatur.')
        """

        mwrite = self._file.write

        mwrite(self.indent + text + '\\\\\n\n')

    def newpage(self):
        """Adding a  \newpage tag

        Examples
        --------
        >>> report.newpage()
        """

        self._file.write(self.indent + '\\newpage\n\n')

    def figure(self, path, **kwargs):
        """Adding figure to the document

        Parameters
        ----------
        path : str
            Relative path to the figure
        specifier : str, optional
            Figure specifier, e.g., 'h' or 'h!'
        attrs : str, optional
            Includegraphics attributes, e.g., 'width=7cm'
        caption : str, optional
            Figure caption

        Returns
        -------
        label : str
            The lable of the figure

        Examples
        --------
        >>> report.figure(
                '/path/to/figure'
                specifier='ht',
                attrs='width=0.6\\textwidth',
                caption='Excepteur sint occaecat cupidatat non proident')
        >>>> report.finish()
        """
        k = kwargs.get

        specifier = k('specifier') if 'specifier' in kwargs else 'h'
        attrs = k('attrs') if 'attrs' in kwargs else ''

        mwrite = self._file.write

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
        """Adding a table of keys and values of a dictionary

        Parameters
        ----------
        dictionary : dict
            Set of keywords values for filling the table
        specifier : str, optional
            Table specifier, e.g., 'h' or 'h!'
        caption : str, optional
            Table caption
        excludekeys : list of str
            A list of keys to be filtered

        Returns
        -------
        label : str
            The lable of the table

        Examples
        --------
        >>> report.dict2table(
                dictionary,
                specifier='ht',
                excludekeys=['key1', 'key2'])
        >>>> report.finish()
        """

        k = kwargs.get

        specifier = k('specifier') if 'specifier' in kwargs else 'h!'
        excludekeys = k('excludekeys') if 'excludekeys' in kwargs else []

        mwrite = self._file.write

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
        """Finishing the document by adding bibliography (if available)

        Parameters
        ----------
        refrencese : str
            Path to the refrences bib file

        Examples
        --------
        >>>> report.finish(refrences='/path/to/bib/file')
        """

        mwrite = self._file.write

        if 'refrences' in kwargs:
            mwrite('\n\t\\bibliography{' + kwargs.get('refrences') + '}{}\n')
            mwrite('\t\\bibliographystyle{plain}\n\n')

        mwrite('\\end{document}\n')

        self._file.close()

def labelgenerator(prefix, size=10, chars=string.ascii_letters + string.digits):
    """Generating label for figures, tables, etc.

    Parameters
    ----------
    prefix : str
        Label prefix
    size : int, optional
        Length of the generated label
    chars : str
        A string containing all valid charachters

    Returns
    -------
    str
        Generated label
    """
    return prefix + ''.join(random.choice(chars) for _ in range(size))

def safetext(text):
    """Returning a latex safe text

    Parameters
    ----------
    text : str

    Returns
    -------
    str
    """
    return text.replace('_', '\\_').replace("'", '')
