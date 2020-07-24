# this is the shell command that gets executed every time this widget refreshes
command: "${HOME}/bin/mac_check_wireless_battery"

# the refresh frequency in milliseconds
refreshFrequency: 30000

# render gets called after the shell command has executed. The command's output
# is passed in as a string. Whatever it returns will get rendered as HTML.
render: (output) -> """
  <h1>#{output}</h1>
"""

# the CSS style for this widget, written using Stylus
# (http://learnboost.github.io/stylus/)
style: """
  -webkit-backdrop-filter: blur(2px)
  border-radius: 5px
  border: 1px solid #fff
  box-sizing: border-box
  color: #DC143C
  font-family: Glass TTY VT220
  font-weight: 300
  left: 100%
  line-height: 1.5
  margin-left: -295px
  padding: 7px 7px 7px
  top: 94%
  text-align: justify

  h1
    font-size: 16px
    font-weight: 300
    margin: 16px 0 8px

  em
    font-weight: 400
    font-style: normal
"""
