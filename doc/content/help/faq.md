# Frequently Asked Questions

!css style=font-size:200%;font-weight:300
What if the `git clone` command does nothing?

Depending on your network configuration the HTTP access to the repository may not work. In this case
you must use [SSH](https://en.wikipedia.org/wiki/Secure_Shell) keys. First, follow the instructions to [generate a new SSH key](https://help.github.com/articles/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent/#generating-a-new-ssh-key) and then run the following clone command.


```
git clone git@github.com:idaholab/mastodon.git
```
