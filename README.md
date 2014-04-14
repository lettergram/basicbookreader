BasicBookReader
============

A book reader written in C++ and Qt 5.2 (Qt 4+ should work)

Required: <a href="http://qt-project.org/downloads">Qt 4.+</a> and C++11 

Excellent Book Source: Project Gutenberg - http://www.gutenberg.org/

Platform: Cross-Platform

<h2>Purpose:</h2>

I built basicbookreader as a template for other book readers, the goal specifically in mind
is adding a large number of analytics other book readers did not offer (or at least did not let me control).
This reader only took ~20 hours to complete at this point and I am sure you can tell, please feel free to offer suggestions or make pull requests, etc. The reason I am making this open source is the hope that someone can use it, if 
they ever stumble upon my github that is.

If anyone does end up using it, I would hope you contribute to the project, or at least let me know what you use it for! It's nice to know when a project is being put to use.

<h2>Directions:</h2>

  <h3>Build:</h3>
  <ul>
    <li>Download basicbookreader</li>
    <li>Install Qt 5+ </li>
    <li>Open Qt Creator</li>
    <li>Click File->Open file or project</li>
    <li>Select the ".pro" file</li>
    <li>Click the big Green Arrow and run your program (this should build it)</li>
  </ul>
  <h3>Run:</h3>
    <ul>
      <li>**Important** - If you decide to build and run a"release" version of Qt, 
      make sure that your books folder is inside folder. Alternatively, you could copy
      the books folder to the parent directory, basicbookreader will search the parent directory
      for "/books" as well.</li>
      <li>Launch the program once your books folder is set up correctly, by default you have Tom Sawyer.
      If you would like to launch a different book replace the name with the file you wish to load.</li>
      <ul>
        <li>At the moment only .txt files are supported (Project Gutenberg has .txt files free to download)</li>
        <li>Click add to library to add it to your library.</li>
      </ul>
    <li>Once the book is added to the library if you would like to switch to your old book you can:</li>
      <ul>
        <li>Replace the current book name with the book you wish to switch to (make sure to bookmark if you want to 
        keep your page). </li>
        <li>Once you replace the name click "checkout book" and you will checkout the book from the library.</li>
      </ul>
    </ul>  
  <h3>Features:</h3>
     <ul> 
    <li>**Saved book locations**, the first load is slow. This is because we index all of the pages. After the first
      time you load a book it is saved in a file. Since I decided to keep this as lean as possible every time you
      launch the application it will load library.txt which will contain all the titles you currently have, plus 
      all indexes. This means that when you launch a book it should load instantly (since we only load one page at 
      a time - 50 lines).</li>
    <li>**Ease of use**, I intended this application to be as easy to use and as portible as possible. I didn't want 
      to have to worry about any complexities with file formats or notes. This design decision means you cannot 
      use  many features you may be use to with your current ebook readers. However, it also means I developed an 
      easily expandable and usable application, which other programmers can use as a templete to build a better 
      ebook reader. Further, something this simple is easy to maintain, repair and hard to break. I would rather 
      do one thing correcly than a lot poorly.</li>
    <li>**Low space requirement**, I hate programs that are slow and too big. Small, sleek, and quick is the only
      way to go in my opinion.</li>
    <li>**Data, data, data**, because I leave the hood open and all of the data is open source, you have access to
      all available data and can build libraries for better analytics. This was and is my intended use.</li>
    </ul> 
    
    
<h2>Liscense</h2>
<b>The MIT License (MIT)</b>

Copyright (c) 2014 Austin G. Walters

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
