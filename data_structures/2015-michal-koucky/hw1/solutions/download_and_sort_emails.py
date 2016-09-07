#!/usr/bin/python
import mailbox
import re
import email
import email.parser
import os

MAILDIR_PATH="./Maildir/INBOX"
SOLUTION_PATH="./solutions"

def read_and_sort_mails():
	md = mailbox.Maildir(MAILDIR_PATH, factory=email.message_from_file)
	r = re.compile('HW1?-(\d+)')
	parser = email.parser.Parser()
	for msg in reversed(list(md)):
		match = r.match(msg['Subject'])
		if not match:
			continue
	
		student_id = match.group(1)
		filename = None
		source = None
		text = None
		for part in msg.walk():
			if part.get_content_type() == "text/plain" and part.get('Content-Disposition') is None:
				text = part.get_payload(decode=True)
	
			if part.get_content_maintype() == 'multipart':
		            continue
		        if part.get('Content-Disposition') is None:
		            continue

			filename=part.get_filename()
		        if filename is None:
				continue
			source = part.get_payload(decode=True)

		try:
			os.makedirs(SOLUTION_PATH + os.sep + student_id)
		except:
			pass

		if text:
			f = open(SOLUTION_PATH + os.sep + student_id + os.sep + 'mail', 'w')
			f.write(text)
			f.close()

			for line in text.split("\n"):
				l = line.split(":", 2)
				if len(l) != 2:
					continue
				key, value = l
				key = key.strip()
				value = value.strip().lower()
				if key == 'kompilace' or key.lower() == 'compile with':
					f = open(SOLUTION_PATH + os.sep + student_id + os.sep + 'compile.sh', 'w')
					f.write(value)
					f.close()
					
		
		if filename:
			f = open(SOLUTION_PATH + os.sep + student_id + os.sep + filename, 'w')
			f.write(source)
			f.close()


if __name__ == "__main__":
	read_and_sort_mails()

