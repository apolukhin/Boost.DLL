
find . -type f -iname "*.html" | while read i; do
    echo "Processing: $i"
    sed -i "s#</body>#\
    <script>\n\
      (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){\n\
      (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),\n\
      m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)\n\
      })(window,document,'script','//www.google-analytics.com/analytics.js','ga');\n\
\n\
      ga('create', 'UA-64524257-1', 'auto');\n\
      ga('send', 'pageview');\n\
\n\
    </script>\n\
</body>#g" "$i"

done
