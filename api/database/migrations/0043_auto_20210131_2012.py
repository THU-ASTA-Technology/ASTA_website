# Generated by Django 3.1.4 on 2021-01-31 20:12

from django.conf import settings
from django.db import migrations, models
import django.db.models.deletion


class Migration(migrations.Migration):

    dependencies = [
        ('database', '0042_auto_20210102_1912'),
    ]

    operations = [
        migrations.AlterField(
            model_name='problem',
            name='author',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.SET_NULL, to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='problem',
            name='contest',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.SET_NULL, related_name='problems', to='database.contest'),
        ),
        migrations.AlterField(
            model_name='problemhighestscore',
            name='team',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='highestScore', to='database.team'),
        ),
        migrations.AlterField(
            model_name='problemhighestscore',
            name='user',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.CASCADE, related_name='highestScore', to=settings.AUTH_USER_MODEL),
        ),
        migrations.AlterField(
            model_name='problemsubmission',
            name='team',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.SET_NULL, to='database.team'),
        ),
        migrations.AlterField(
            model_name='problemsubmission',
            name='user',
            field=models.ForeignKey(blank=True, null=True, on_delete=django.db.models.deletion.SET_NULL, to=settings.AUTH_USER_MODEL),
        ),
        migrations.CreateModel(
            name='ProblemJudgeDetail',
            fields=[
                ('id', models.AutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('score', models.FloatField(default=0)),
                ('status', models.CharField(default='', max_length=20)),
                ('timeUsed', models.FloatField(default=0)),
                ('submission', models.ForeignKey(null=True, on_delete=django.db.models.deletion.CASCADE, related_name='detail', to='database.problemsubmission')),
            ],
        ),
    ]