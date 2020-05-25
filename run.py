import sys
import os
import subprocess


def run(correct_answers):
    subprocess.Popen(['make']).wait()

    if not os.path.exists('output'):
        os.makedirs('output')

    for instance in sorted(os.listdir('instances')):
        if instance.endswith(".inp"):
            output_path = 'output/' + instance[:-len('inp')] + 'out'
            with open('instances/' + instance, 'r') as instance_f, open(output_path, 'w') as output_f:
                print("Running", instance)
                process = subprocess.Popen(['./BWP'], stdin=instance_f, stdout=output_f)
                try:
                    process.wait(timeout=60)
                    if instance in correct_answers:
                        with open('instances/' + instance, 'r') as instance_f_read, open(output_path, 'r') as output_f_read:
                            assert correct_answers[instance] == output_f_read.readlines()[len(instance_f_read.readlines())]
                except subprocess.TimeoutExpired:
                    print("Timeout expired")
                    os.remove(output_path)


def get_correct_answers():
    with open('results.txt') as results:
        answers = {}
        for line in results:
            if line.startswith('==>'):
                instance_name = line[4:-5]
            elif len(line) > 1:
                answers[instance_name] = line
        return answers


def main():
    correct_answers = get_correct_answers()
    run(correct_answers)

main()