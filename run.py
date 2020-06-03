import sys
import os
import subprocess


def run(path, correct_answers):
    subprocess.Popen(['make'], cwd=path).wait()

    if not os.path.exists(path + '/output'):
        os.makedirs(path + '/output')

    total_instances = len(os.listdir('instances'))
    total_answers = len(correct_answers)
    count_finish = 0
    count_correct = 0
    count_timeout = 0

    for instance in sorted(os.listdir('instances')):
        if instance.endswith(".inp"):
            output_path = path + '/output/' + instance[:-len('inp')] + 'out'
            with open('instances/' + instance, 'r') as instance_f, open(output_path, 'w') as output_f:
                print("Running", instance)
                process = subprocess.Popen(['./BWP'], cwd=path, stdin=instance_f, stdout=output_f)
                try:
                    process.wait(timeout=60)
                    count_finish += 1
                    with open(output_path, 'r') as output_f_read:
                        checker = subprocess.Popen(['../checker'], cwd=path, stdin=output_f_read, stdout=subprocess.PIPE)
                        checker_output = checker.communicate()[0].decode("utf-8")
                        print(checker_output)
                        assert checker_output[:2] == 'OK'
                    with open('instances/' + instance, 'r') as instance_f_read, open(output_path, 'r') as output_f_read:
                        if instance in correct_answers:
                            assert correct_answers[instance] == output_f_read.readlines()[len(instance_f_read.readlines())]
                            count_correct += 1
                    
                except subprocess.TimeoutExpired:
                    print("Timeout expired")
                    count_timeout += 1
                    process.kill()
                    os.remove(output_path)
            print(f'Finished: {count_finish}/{total_instances}, Checked: {count_correct}/{total_answers}, Timeout: {count_timeout}/{total_instances}')


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
    if len(sys.argv) != 2:
        print('Usage: python run.py {1_constraint_programming, 2_linear_programming, 3_sat}')
        return 1
    path = sys.argv[1]
    correct_answers = get_correct_answers()
    run(path, correct_answers)

main()